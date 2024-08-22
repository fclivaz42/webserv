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

//SETTING UP THE SOCKET MANAGER
// Initialise le fd pour le server scoket a -1 pour indiquer que le socket n'a pas ete cree
// Initialise un port par defaut a 8080
// clear memoire pour serverAddress et set to les bytes a 0 pour eviter les garbage data
// set l'address family a AF_INET ce qui veut dire qu'on utilise le protocole IPv4
// set le server sur INADDR_ANY pour binder le server a toutes les network interfaces available
// htons(_port) convertis le numero de port de host byte a network byte order pour pouvoir lire sur tous les systemes
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
//CREATE A SOCKET
//Creer une socket avec la fonction socket()
//AF_INET encore pour dire qu'on utiliose IPv4 protocol
//SOCK_STREAM pour dire qu'on utilise un TCP stream 
//Je ste les socket options avec setsockopt()
//SOL_SOCKET == option que j'applique au niveau du socket
//SO_REUSEADDR == autorise le server a reutilise l'addresse ip et le port si deja utilise.
//Utile quand on restart le server et que l'adresse est toujours en TIME_WAIT state.
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

//BIND A SOCKET
//j'assigne les bonnes infos contrairement au constructeur qui assignauit les valeurs par default
//address info + ip address + port number
//Je binde le socket a l'adresse du server avev bind()
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

//FUNCTION TO PUT THER SERVER SOCKET IN A LISTEN STATE
//je commmence a ecouter des connections avec la fonction listen()
//backlog corresponds au nombre max de connections en attentes qui peuvent etre queue dans ma socket
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

//FUNCTION TO ACCEPT INCOMING CONNECTIONS FROM CLIENT TO SERVER
//quand un client essaye de se connecter a un server, cette fonction gere la requete de connection
//et etablie une nouvelle connexion.
//sockaddr__in est une struct utilise pour storer les infos du client comme l'ip address et le num de port
//clientLen stores la size de clientAddress
//que je passe a accept() qui est le system call qui accept les connections entrantes d'un client
//La fonction retourne le clientFd qui represents la new co et print l'ip address client
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

//FONCTION READ MESSAGE
//Fonction responsable pour lire la data envoyee par le client via la socket connexion
// Je set un buffer pour contenir les datas et une taille max pour definir le nombre de data lu en une fois
// avec read() je lis le contenue de mon buffer
// Je checke si read etait successful
// si oui je convertis en string et je return.
// Si le client s'est deco, je ne return rien
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
}

//FUNCTION START MANAGES MULTIPLE CLIENT CONNECTIONS USING POLL()
//Le server se tart or faisant une liste de fd qui seront gerers par poll()
//La loop principale checks non stop si il y a une activite via les fds
//Quand un nouveau client se connecte, il est ajoute a la liste the fd monitored
//Quand un client envoie de la data, le server lit la data, la process et s'occupe de la deconnexion
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

//FUNCTION TO PROCESS THE INCOMING DATA FROM THE CONNECTED CLIENT
//S'occupe de requetes HTTP et gere les connexions persistantes basees sur les headers HTTPS
//J'assigne un flag keepAlive pour savoir si je dois garder la connexion ouvertes ou non en cas de requetes multiples
//Je boucle tant que la connection est en keep alive et je lis les datas du client
//Je check si on m'a envoye une requete HTTP ou un message simple
//Si j'ai bien une requete HTTP, j'envoie le bon status code
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
			if (response.find("Connection: close") != std::string::npos)
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

//FUNCTION DESIGNED TO READ CONTENTS OF A FILE
//J'ouvre mon file avec ifstrem
//je convertis en string Cstyle
//Si ouverture du fichier okay je lis le content avec content()
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
