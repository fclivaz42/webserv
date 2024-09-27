// Pas de header pour eviter les conflicts :)

#include "Network/ConnectManager.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/HttpRequest.hpp"
#include "Requests/Get.hpp"
#include "Requests/Post.hpp"
#include "Requests/Delete.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

//SETTING UP THE SOCKET MANAGER
// Initialise le fd pour le server scoket a -1 pour indiquer que le socket n'a pas ete cree
// Initialise un port par defaut a 8080
// clear memoire pour serverAddress et set to les bytes a 0 pour eviter les garbage data
// set l'address family a AF_INET ce qui veut dire qu'on utilise le protocole IPv4
// set le server sur INADDR_ANY pour binder le server a toutes les network interfaces available
// htons(_port) convertis le numero de port de host byte a network byte order pour pouvoir lire sur tous les systemes

ConnectManager::ConnectManager(const Servers& ServerList) : _serverFds(0), _serverPorts(0), _pairPortsNames(0), _serverList(ServerList)
{
	struct	sockaddr_in						servAddrin;
	std::vector<unsigned short>				vecPort;

	for (size_t i = 0; i < (size_t)ServerList.getAmountOfServers(); i++) {
		vecPort = ServerList.getServConf(i).getPort();
		for (size_t j = 0; j < vecPort.size(); j++) {
			ft_bzero(&servAddrin, sizeof(struct sockaddr_in));
			servAddrin.sin_family = AF_INET;
			servAddrin.sin_addr.s_addr = htonl(INADDR_ANY);
			servAddrin.sin_port = htons(vecPort[j]);
			_pairPortsNames.push_back(std::pair<std::string, unsigned short>(ServerList.getServConf(i).getServerName(),vecPort[j]));
			_serverPorts.push_back(servAddrin);
			_port.push_back(vecPort[j]);
		}
	}

	if (DEBUG)
		for (size_t j = 0; j < _pairPortsNames.size(); j++)
			std::cout << "Server \'" << _pairPortsNames[j].first << "\' has port: " << _pairPortsNames[j].second << "\n";
}

ConnectManager::~ConnectManager()
{
	for (std::vector<int>::size_type i = 0; i < _serverFds.size(); i++)
		close(_serverFds[i]);
}

void	ConnectManager::closErase(size_t index)
{
	close(_serverFds[index]);
	_serverFds.erase(_serverFds.begin() + index);
	_serverPorts.erase(_serverPorts.begin() + index);
	_port.erase(_port.begin() + index);
	_pairPortsNames.erase(_pairPortsNames.begin() + index);
}

/*
	StartSocketListen comprend 3 etapes:

	1: Creer une socket avec la fonction socket()
		AF_INET encore pour dire qu'on utiliose IPv4 protocol
		SOCK_STREAM pour dire qu'on utilise un TCP stream 
		Je ste les socket options avec setsockopt()
		SOL_SOCKET == option que j'applique au niveau du socket
		SO_REUSEADDR == autorise le server a reutilise l'addresse ip et le port si deja utilise.
		Utile quand on restart le server et que l'adresse est toujours en TIME_WAIT state.

	2: Bind au socket
		j'assigne les bonnes infos contrairement au constructeur qui assignauit les valeurs par default
		address info + ip address + port number
		Je binde le socket a l'adresse du server avev bind()

	3: Demarrer le listen
		je commmence a ecouter des connections avec la fonction listen()
		backlog corresponds au nombre max de connections en attentes qui peuvent etre queue dans ma socket

	NOTE: on fait en sorte d'ecouter quand meme si un seul port bind et arrive a listen. On close() les fd's qui sont inutilisables.
*/

bool	ConnectManager::startSocketListen(int backlog)
{
	for (std::vector<int>::size_type i = 0; i < _port.size(); i++)
	{
		int port = _port[i];
		int serverFd = socket(AF_INET, SOCK_STREAM, 0);
		int opt = 1;

		if (serverFd == -1) {
			std::cerr << RED << "ERROR: Unable to create socket for port " << port << RESET << std::endl;
			_serverPorts.erase(_serverPorts.begin() + i);
			_port.erase(_port.begin() + i);
			_pairPortsNames.erase(_pairPortsNames.begin() + i);
			continue;
		}
		else if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			std::cerr << RED << "ERROR: setsockopt failure for port " << port << RESET << std::endl;
			close(serverFd);
			_serverPorts.erase(_serverPorts.begin() + i);
			_port.erase(_port.begin() + i);
			_pairPortsNames.erase(_pairPortsNames.begin() + i);
			continue;
		}
		if (DEBUG)
			std::cerr << GREEN << "SFD for port " << _port[i] << ": " << serverFd << RESET << std::endl;
		_serverFds.push_back(serverFd);
	}
	
	if (_serverFds.empty())
		return (false);

	for (size_t i = 0; i < _port.size(); i++)
	{
		if (bind(_serverFds[i], (struct sockaddr*)&_serverPorts[i], sizeof(_serverPorts[i])) == -1)
		{
			std::cerr << RED << "ERROR: Failed to bind SFD " << _serverFds[i] << " to port " << ntohs(_serverPorts[i].sin_port) << RESET << std::endl;
			closErase(i);
			--i;
			continue;
		}
		if (DEBUG)
			std::cerr << GREEN << "BOUND SFD " << _serverFds[i] << " to port " << ntohs(_serverPorts[i].sin_port) << RESET << std::endl;
	}

	if (_serverFds.empty())
		return (false);

	for (size_t i = 0; i < _serverFds.size(); i++)
	{
		if (listen(_serverFds[i], backlog) == -1)
		{
			std::cerr << RED << "ERROR: Could not listen on port " << _port[i] << ". It may already be in use." << RESET << std::endl;
			closErase(i);
			--i;
			continue;
		}
		std::cout << GREEN << "Server" << " listening on port " << _port[i] << RESET << std::endl;
	}
	
	if (_serverFds.empty())
		return (false);

	return (true);
}

//FUNCTION TO ACCEPT INCOMING CONNECTIONS FROM CLIENT TO SERVER
//quand un client essaye de se connecter a un server, cette fonction gere la requete de connection
//et etablie une nouvelle connexion.
//sockaddr__in est une struct utilise pour storer les infos du client comme l'ip address et le num de port
//clientLen stores la size de clientAddress
//que je passe a accept() qui est le system call qui accept les connections entrantes d'un client
//La fonction retourne le clientFd qui represents la new co et print l'ip address client
int	ConnectManager::acceptConnection(int serverFd, std::vector<unsigned short>& portlist)
{
	struct sockaddr_in clientAddress;
	socklen_t clientLen = sizeof(clientAddress);
	struct sockaddr_in serverPort;
	socklen_t portLen = sizeof(serverPort);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &clientLen);
	if (clientFd == -1 || getsockname(serverFd, (struct sockaddr*)&serverPort, &portLen) == -1)
		std::cerr << RED << "\nERROR : Connection failure" << RESET << std::endl;
	if (DEBUG)
		if (clientFd > 0)
			std::cerr << GREEN << "\n┌──────────\n│ New connection from: " << inet_ntoa(clientAddress.sin_addr) << "\n└──────────" << RESET << std::endl;
	portlist.push_back(ntohs(serverPort.sin_port));
	return (clientFd);
}

//FONCTION READ MESSAGE
//Fonction responsable pour lire la data envoyee par le client via la socket connexion
// Je set un buffer pour contenir les datas et une taille max pour definir le nombre de data lu en une fois
// avec read() je lis le contenue de mon buffer
// Je checke si read etait successful
// si oui je convertis en string et je return.
// Si le client s'est deco, je ne return rien
std::string	ConnectManager::readMessage(int clientFd)
{
	std::string	message;
	char		buffer[BUFFER_SIZE];
	ssize_t		bytesRead;

	while (true)
	{
		bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			message.append(buffer);
			if (message.find("\r\n\r\n") != std::string::npos)
				break ;
		}
		else if (bytesRead == 0)
			break ;
		else
		{
			std::cerr << RED << "ERROR: read() failure" << RESET << std::endl;
			return ("");
		}
	}
	return (message);
}

//FUNCTION START MANAGES MULTIPLE CLIENT CONNECTIONS USING POLL()
//Le server se tart or faisant une liste de fd qui seront gerers par poll()
//La loop principale checks non stop si il y a une activite via les fds
//Quand un nouveau client se connecte, il est ajoute a la liste the fd monitored
//Quand un client envoie de la data, le server lit la data, la process et s'occupe de la deconnexion
void	ConnectManager::start()
{
	std::vector<struct pollfd>	fds(0);
	std::vector<unsigned short>	portlist(0);

	for (size_t i = 0; i < _serverFds.size(); i++)
	{
		struct pollfd serverPollFd;
		serverPollFd.fd = _serverFds[i];
		serverPollFd.events = POLLIN;
		serverPollFd.revents = 0;
		portlist.push_back(0);
		fds.push_back(serverPollFd);
	}

	while (true)
	{
		int pollCount = poll(fds.data(), fds.size(), -1);
		if (pollCount < 0)
		{
			std::cerr << RED << "ERROR: poll() failure" << RESET << std::endl;
			continue ;
		}
		for (size_t i = 0; i < _serverFds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				int clientFd = acceptConnection(_serverFds[i], portlist);
				if (clientFd >= 0)
				{
					struct pollfd clientPollFd;
					clientPollFd.fd = clientFd;
					clientPollFd.events = POLLIN;
					clientPollFd.revents = 0;
					fds.push_back(clientPollFd);
				}
			}
		}
		std::cout << "PORTLIST: \n";
		for (size_t i = 0; i < portlist.size(); i++)
			std::cout << "PORT: " << portlist[i] << "\n";
		for (size_t i = _serverFds.size(); i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				for (size_t n = 0; n < (size_t)this->_serverList.getAmountOfServers(); n++)
				{
					if (std::find(_serverList.getServConf(n).getPort().begin(), _serverList.getServConf(n).getPort().end(), portlist[i]) != _serverList.getServConf(n).getPort().end())
					{
						const ServerConf&	serverConf = _serverList.getServConf(n);
						std::cout << "CLIENT " << fds[i].fd << " ON PORT " << portlist[i] << " IS USING SERVER " << serverConf.getServerName() << "\n";
						handleClient(fds[i].fd, serverConf);
					}
				}
				portlist.erase(portlist.begin() + i);
				fds.erase(fds.begin() + i);
				i--;
			}
		}
	}
}

//FUNCTION TO STORE REQUEST FROM CLIENT INTO HTTPREQUEST CLASS
void	ConnectManager::handleClient(int clientFd, const ServerConf& serverConf)
{
	std::string message = readMessage(clientFd);
	size_t headerEnd = message.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
	    std::string headers = message.substr(0, headerEnd);
	    std::string body = message.substr(headerEnd + 4);  // Skip the "\r\n\r\n"
	}
	std::string response;
	//if (message.empty())
	//{
	//	std::cerr << RED << "Client disconnected or empty message" << RESET << "" << std::endl;
	//	close(clientFd);
	//}
	try
	{
		HttpRequest request = HttpRequest(message);
		if (request.getMethod() == "GET")
			response = processGetRequest(request, serverConf);
		else if (request.getMethod() == "POST")
			response = processPostRequest(request, serverConf);
		else if (request.getMethod() == "DELETE")
			response = processDeleteRequest(request);

	}
	catch (const std::exception& error)
	{
		std::cerr << "Request parsing failed " << error.what() << std::endl;
		response = "HTTP/1.1 400 Bad Request\r\n\r\n" + std::string(error.what());
		//TODO send response error back to client
	}
	ssize_t bytesWritten = write(clientFd, response.c_str(), response.length());
	close(clientFd);
	if (bytesWritten == -1)
	{
		std::cerr << RED << "ERROR: write() failure" << RESET << std::endl;
		close(clientFd);
	}
	else if (bytesWritten != static_cast<ssize_t>(response.length())) 
	{
		std::cerr << RED << "ERROR: Failure to write all datas" << RESET << std::endl;
		close(clientFd);
	}
}

//FUNCTION DESIGNED TO READ CONTENTS OF A FILE
//J'ouvre mon file avec ifstrem
//je convertis en string Cstyle
//Si ouverture du fichier okay je lis le content avec content()
std::string	ConnectManager::readFile(const std::string& filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Could not open file " << filePath << std::endl;
		return ("");
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return (content);
}

//GETTERS
const std::vector<int>&	ConnectManager::getServerFd() const
{
	return (_serverFds);
}
