// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   SocketManager.cpp                                  :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 11:15:07 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 11:32:36 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "../include/SocketManager.hpp"

SocketManager::SocketManager() : _serverFd(-1)
{
	memset(&_serverAddress, 0, sizeof(_serverAddress));
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

bool	SocketManager::bindSocket(int port)
{
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = INADDR_ANY;
	_serverAddress.sin_port = htons(port);

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

int	SocketManager::readMessage(int clientFd)
{
	while (true)
	{
		char	buffer[BUFFER_SIZE];
		ssize_t	bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
		
		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			std::cout << PURPLE << "Received message !! >> " << buffer << RESET << std::endl;
		}
		else if (bytesRead == 0)
		{
			std::cout << ORANGE << "Client disconnected. . ." << RESET << std::endl;
			return (-1);
		}
		else
		{
			std::cerr << RED << "ERROR: read() failure" << RESET << std::endl;
			return (-1);
		}
	}
	close(clientFd);
	return (0);
}

//GETTERS
int	SocketManager::getServerFd() const
{
	return (_serverFd);
}
