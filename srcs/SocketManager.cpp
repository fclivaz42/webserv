// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   SocketManager.cpp                                  :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 11:15:07 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 14:37:37 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "../include/SocketManager.hpp"
#include "../include/HttpRequestHandler.hpp"

SocketManager::SocketManager() : _serverFd(-1), _port(8080)
{
	memset(&_serverAddress, 0, sizeof(_serverAddress));
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = INADDR_ANY;
	_serverAddress.sin_port = htons(_port);
}

SocketManager::~SocketManager()
{
	if (_serverFd == -1)
		close(_serverFd);
}


//METHODS
bool	SocketManager::createSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << RED << "ERROR: Unable to create socket" << RESET << std::endl;
		return (false);
	}
	
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << RED << "ERROR: setsockopt failure" << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool	SocketManager::bindSocket()
{
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = inet_addr(_host.c_str());
	_serverAddress.sin_port = htons(_port);

	if (bind(_serverFd, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		std::cerr << RED << "ERROR: Binding failure" << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool	SocketManager::startListening(int backlog)
{
	if (listen(_serverFd, backlog) == -1)
	{
		std::cerr << RED << "ERROR: Listening failure" << RESET << std::endl;
		return (false);
	}
	std::cout << GREEN << "Server listening on port " << ntohs(_serverAddress.sin_port) << RESET << std::endl;
	return (true);
}

int	SocketManager::acceptConnection()
{
	struct sockaddr_in clientAddress;
	socklen_t clientLen = sizeof(clientAddress);
	int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddress, &clientLen);

	if (clientFd == -1)
	{
		std::cerr << RED << "ERROR : Connection failure" << RESET << std::endl;
	}
	else
	{
		std::cerr << GREEN << "New connection from " << inet_ntoa(clientAddress.sin_addr) << RESET << std::endl;
	}
	return (clientFd);
}

std::string	SocketManager::readMessage(int clientFd)
{
	char		buffer[BUFFER_SIZE];
	ssize_t	bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
	
	if (bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		return (std::string(buffer));
	}
	else if (bytesRead == 0)
	{
		std::cout << ORANGE << "Client disconnected. . ." << RESET << std::endl;
		return ("");
	}
	else
	{
		std::cerr << RED << "ERROR: read() failure" << RESET << std::endl;
		return ("");
	}
	return (std::string(buffer));
}

int	SocketManager::start()
{
	std::vector<struct pollfd> fds;

	struct pollfd serverPollFd;

	serverPollFd.fd = _serverFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;

	fds.push_back(serverPollFd);

	while (true)
	{
		int pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
		{
			std::cerr << RED << "ERROR: poll() failure" << RESET << std::endl;
			return (-1);
		}
		if (fds[0].revents & POLLIN)
		{
			int clientFd = acceptConnection();
			if (clientFd >= 0)
			{
				struct pollfd clientPollFd;

				clientPollFd.fd = clientFd;
				clientPollFd.events = POLLIN;
				clientPollFd.revents = 0;

				fds.push_back(clientPollFd);
			}
		}
		std::vector<int> closedFds;
		for (size_t i = 1; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				handleClient(fds[i].fd);
				closedFds.push_back(fds[i].fd);
				fds[i].fd = -1;
			}
		}
		for (std::vector<int>::iterator iter = closedFds.begin(); iter != closedFds.end(); iter++)
		{
			for (std::vector<struct pollfd>::iterator fdIter = fds.begin(); fdIter != fds.end(); fdIter++)
			{
				if (fdIter->fd == *iter)
					fdIter = fds.erase(fdIter);
				else
					fdIter++;
			}
		}
	}
}

bool	SocketManager::isHttpRequest(const std::string& message)
{
	return (message.find("GET ") == 0 ||
			message.find("POST ") == 0 ||
			message.find("DELETE ") == 0 ||
			message.find("PUT ") == 0 ||
			message.find("HEAD ") == 0 ||
			message.find("OPTIONS ") == 0);
}

void	SocketManager::handleClient(int clientFd)
{
	bool keepAlive = true;
	while (keepAlive)
	{
		std::string message = readMessage(clientFd);
		if (message.empty())
		{
			close(clientFd);
			break ;
		}

		if (isHttpRequest(message))
		{
			std::string response = HttpRequestHandler::handleRequest(message);

			ssize_t bytesWritten = write(clientFd, response.c_str(), response.length());

			if (bytesWritten == -1)
			{
				std::cerr << RED << "ERROR: Write() failure" << RESET << std::endl;
				close(clientFd);
				break ;
			}
			else if (bytesWritten != static_cast<ssize_t>(response.length())) 
			{
				std::cerr << RED << "ERROR: Failure to write all datas" << RESET << std::endl;
				close(clientFd);
				break ;
			}
			if (message.find("Connection: close") != std::string::npos)
			{
				keepAlive = false;
			}
		}
		else
		{
			std::string fileContent = readFile("index.html");
			std::string response = "HTTP/1.1 200 OK\r\n Content-Type: text/htmlr\n\r\n" + fileContent;
			ssize_t bytesWritten = write(clientFd, response.c_str(), response.length());
			if (bytesWritten == -1)
			{
				std::cerr << RED << "ERROR: write() failure" << RESET << std::endl;
				close(clientFd);
				break ;
			}
			else if (bytesWritten != static_cast<ssize_t>(response.length())) 
			{
				std::cerr << RED << "ERROR: Failure to write all datas" << RESET << std::endl;
				close(clientFd);
				break ;
			}
			keepAlive = false;
		}
	}
}

std::string	SocketManager::readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cerr << "ERROR: Could not open file " << filePath << std::endl;
		return ("");
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return (content);
}

//GETTERS
int	SocketManager::getServerFd() const
{
	return (_serverFd);
}

//SETTERS
void	SocketManager::setPort(int port)
{
	_port = port;
}

void	SocketManager::setHost(const std::string& host)
{
	_host = host;
}
