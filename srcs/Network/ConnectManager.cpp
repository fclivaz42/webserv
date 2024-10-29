// Pas de header pour eviter les conflicts :)

#include "Network/ConnectManager.hpp"
#include "Requests/HTTPResponse.hpp"
#include "CGI/CGIExec.hpp"
#include <algorithm>
#include <string>

/*
	c tipar pour le construiseur d'une manager de connect
	En vrai ce qui se passe ici c'est qu'on va d'abord initialiser _serverFds, _serverPorts et _port a NULL.
		La raison pour laquelle on fait ca c'est pour eviter d'avoir des vecteurs alloues a leur max_size.
	On va ensuite recuperer nos donnees parsees dans _serverList, comme ca on a acces a tout.

	Pour chaque port de chaque Serveur, on va push_back un sockaddr_in sur _serverPorts.
		On set sin_family sur AF_INET pour preciser le protocole IPv4.
		On set sin_addr.s_addr sur INADDR_ANY pour dire qu'on ecoute partout.
		On ajoute le port sur lequel on veut ecouter sur sin_port.
		Finalement, on push_back le port sur _port pour plus tard.

*/

ConnectManager::ConnectManager(const Servers& ServerList) : _serverFds(0), _serverPorts(0), _port(0), _serverList(ServerList)
{
	struct	sockaddr_in	servAddrin;
	std::vector<ushort>	vecPort;
	std::vector<ushort>	usedPorts;

	for (int i = 0; i < ServerList.getAmountOfServers(); i++) {
		vecPort = ServerList.getServConf(i).getPort();
		for (size_t j = 0; j < vecPort.size(); j++) {
			if (std::find(usedPorts.begin(), usedPorts.end(), vecPort[j]) != usedPorts.end())
				continue;
			ft_bzero(&servAddrin, sizeof(struct sockaddr_in));
			servAddrin.sin_family = AF_INET;
			servAddrin.sin_addr.s_addr = htonl(INADDR_ANY);
			servAddrin.sin_port = htons(vecPort[j]);
			_serverPorts.push_back(servAddrin);
			_port.push_back(vecPort[j]);
			usedPorts.push_back(vecPort[j]);
		}
	}
}

ConnectManager::~ConnectManager()
{
	for (std::vector<int>::size_type i = 0; i < _serverFds.size(); i++)
		close(_serverFds[i]);
}

/*
	Ptite fonction toute nulle qui nettoie un peu StartSocketListen
	En vrai cette fonction va close() le fd si le socket fail ou bien si on arrive pas a bind.
	Ensuite, on va erase les fd et ports associes, histoire d'etre plus propres pour la suite
*/

void	ConnectManager::closErase(size_t index)
{
	close(_serverFds[index]);
	_serverFds.erase(_serverFds.begin() + index);
	_serverPorts.erase(_serverPorts.begin() + index);
	_port.erase(_port.begin() + index);
}

/*
	StartSocketListen comprend 3 etapes et returne true ou false selon certains criteres:

	1ere boucle for:
		On va creer un socket pour chaque port dans notre config.
		AF_INET pour dire qu'on utilise IPv4 et SOCK_STREAM pour dire qu'on utilise le protocle TCP.
		Ensuite, setsockopt() va mettre en place quelque regles:
		SOL_SOCKET == option appliquee au niveau du socket
		SO_REUSEADDR == autorise le server a reutilise l'addresse ip et le port si deja utilise.
		Utile quand on restart le server et que l'adresse est toujours en TIME_WAIT state.

	Quand cette etape est terminee, on va check si _serverFds contient des valeurs.
		Si oui, cela veut dire qu'on a pu creer des sockets, donc on continue.
		Si non, cela veut dure que tout a fail donc ciao on se casse d'ici et on return false.

	2eme boucle for:
		On va bind() chaque fd qu'on a a son port respectif.
			Si bind fail, on close() le socket et on nettoie nos donnes.
			Si bind reussit, yippie on ecoute sur le port qu'on lui a donne.

	Quand cette etape est terminee, on va de nouveau check si _serverFds contient des valeurs.
		Si oui, cela veut dire qu'on a pu correctement bind() AU MOINS un port a son fd, donc on continue.
		Si non, cela veut dure que on n'a rien pu bind() donc ciao on se casse d'ici et on return false.

	3eme boucle for:
		On utilise listen() pour demarrer l'ecoute sur les fds qu'on lui a donne.
			Si listen() fail, ca veut dire que le port est deja utilise. On close le fd et erase nos donnes.
			Si listen() reussit, on a enfin fini la chaine port > socket > fd et donc on peut read/write.
			backlog correspond au nombre max de connections en attente sur un socket. (il set a SOMAXCONN de base.)

	Quand tout est fini, on re-check si _serverFds contient des valeurs.
		Si oui, on a pu listen() au moins un port et on demarre webserv en retournant true.
			Par ailleurs, on clear _port, qui n'est plus necessaire.
		Si non, tout a fail et on se casse d'ici avec un return false.

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
			continue;
		}
		else if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			std::cerr << RED << "ERROR: setsockopt failure for port " << port << RESET << std::endl;
			close(serverFd);
			_serverPorts.erase(_serverPorts.begin() + i);
			_port.erase(_port.begin() + i);
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
			std::cerr << RED << "ERROR: Failed to bind SFD " << _serverFds[i] \
				<< " to port " << ntohs(_serverPorts[i].sin_port) << RESET << std::endl;
			closErase(i);
			--i;
			continue;
		}
		if (DEBUG)
			std::cerr << GREEN << "BOUND SFD " << _serverFds[i] << " to port " \
				<< ntohs(_serverPorts[i].sin_port) << RESET << std::endl;
	}

	if (_serverFds.empty())
		return (false);

	for (size_t i = 0; i < _serverFds.size(); i++)
	{
		if (listen(_serverFds[i], backlog) == -1)
		{
			std::cerr << RED << "ERROR: Could not listen on port " << _port[i] \
				<< ". It may already be in use." << RESET << std::endl;
			closErase(i);
			--i;
			continue;
		}
		std::cout << GREEN << "Server" << " listening on port " << _port[i] << RESET << std::endl;
	}
	
	if (_serverFds.empty())
		return (false);

	_port.clear();
	return (true);
}

/*
	readMessage va etre appele par start() tant revents de clientFd est sur POLLIN.
	Il va write ce qu'il a lu dans la stringstream message.
 */

ssize_t	ConnectManager::readMessage(int clientFd, std::string *message)
{
	char		buffer[BUFFER_SIZE];
	ssize_t		bytesRead;

	ft_bzero(buffer, BUFFER_SIZE);
	bytesRead = read(clientFd, buffer, BUFFER_SIZE);
	if (bytesRead > 0) {}
		message->append(buffer, bytesRead);
	if (bytesRead < 0)
		std::cerr << RED << "ERROR: read() failure" << RESET << std::endl;
	return bytesRead;
}

bool	isCGI(HTTPRequest &request){
	
	if (request.getMethod() == "GET"){
		size_t pos = request.getCreatedPath().find("?");
		if (pos == std::string::npos) {
    	return false;
		}
		std::string subPath = request.getCreatedPath().substr(0, pos);
		request.setCreatedPath(subPath);
	}
	std::cout << "CREATED PATH: " << request.getCreatedPath() << " FILENAME : " << request.getFileName() << std::endl;
	if (request.getLoc().getFastcgiIndex().size() == 0)
		return (false);
	if (request.getCreatedPath().rfind(request.getLoc().getFastcgiIndex()) == request.getCreatedPath().size() - request.getLoc().getFastcgiIndex().size())
		return (true);
	else
		return (false);
}
//FUNCTION TO STORE REQUEST FROM CLIENT INTO HTTPREQUEST CLASS
bool	ConnectManager::handleClient(struct pollfd clientFd, const ServerConf& serverConf, std::string& message)
{2
	std::string response;
	
	try
	{
		HTTPRequest request(message, serverConf, _continue);
		request.createPath(request.getPath(), "GET", false);
		std::map<std::string, std::string>	headers = request.getHeaders();
		if (headers["Expect"] == "100-continue") {
			if (request.getContentLength() <= serverConf.getMaxBodySize())
				response = HTTPResponse::generateResponse(100, "", request.isKeepAlive(), request);
			else
				HTTPResponse::generateResponse(417, serverConf.getErrorPath(), "Connection: close", request);
		}
		else if (isCGI(request) == true){

			CGIExec cgi(request);
			int status = cgi.execute();
			std::cout << "STAT: " << status << std::endl; 
			if (status == 500)
				HTTPResponse::generateResponse(500, serverConf.getErrorPath(), request.isKeepAlive(), request);
			else{
				std::string contentType = cgi.getCgiContentType();
                std::string body = cgi.getBody();
                
				response = "HTTP/1.1 200 OK\r\n" + cgi.getHeader() + "\r\n";
				response += "Content-Length: " + std::to_string(cgi.getBody().size()) + "\r\n\r\n";
				response += cgi.getBody();
				std::cout << "Final Response:\n" << response << std::endl;
			}
			
		}
		else if (request.getContentLength() > serverConf.getMaxBodySize())
				HTTPResponse::generateResponse(413, serverConf.getErrorPath(), request.isKeepAlive(), request);
		else if (request.getMethod() == "GET")
			response = processGetRequest(request);
		else if (request.getMethod() == "POST")
			response = processPostRequest(request);
		else if (request.getMethod() == "DELETE")
			response = processDeleteRequest(request);
	}
	catch (const std::exception& error) {
		response = error.what();
	}
	ssize_t bytesWritten = write(clientFd.fd, response.c_str(), response.length());
	if (bytesWritten == -1) {
		std::cerr << RED << "ERROR: write() failure" << RESET << std::endl;
		return(close(clientFd.fd));
	}
	else if (bytesWritten != static_cast<ssize_t>(response.length())) {
		std::cerr << RED << "ERROR: Failure to write entire response" << RESET << std::endl;
		return(close(clientFd.fd));
	}
	if (response != "HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\nContent-Length: 0\r\n\r\n")
		return(close(clientFd.fd));
	return (true);
}

/*
	acceptConnection(), more like handleConnection()

	J'ai move pas mal de start() ici dedans pour avoir a eviter de refaire une for loop avec un offset.
	L'autre raison est car comme ca on peut run plusieurs serveurs en parallele.

	On va creer 3 structs. clientAddress stockera les donnes du client, serverPort sera utile pour savoir sur quel
		port le client s'est connecte. clientPfd contiendra le poll fd du client.
	On commence par accept() la connection qu'on a recu sur le serverFd, ce qui retournera le clientPfd.
		On va check viteuf si c'est < 0 pour savoir si on a fail. Dans le meme if, on va getsockname() pour
			recuperer le port sur lequel le client s'est connecte. Si ca fail, on abort la connection.
	Ensuite, on a trouver sur quel serverConf on doit renvoyer le client grace au port qu'on a recupere auparavant.
	Une fois que cela est fait, on lance handleClient() avec le clientFd ainsi que le serverConfig correct.
*/

void	ConnectManager::acceptConnection(int serverFd, std::vector<struct pollfd>& fdList, std::map<int, ushort>& fdMap)
{
	struct sockaddr_in	clientAddress;
	struct sockaddr_in	serverPort;
	socklen_t clientLen = sizeof(clientAddress);
	socklen_t portLen = sizeof(serverPort);
	struct pollfd		clientPfd;

	clientPfd.fd = accept(serverFd, (struct sockaddr*)&clientAddress, &clientLen);
	if (clientPfd.fd == -1 || getsockname(serverFd, (struct sockaddr*)&serverPort, &portLen) == -1) {
		std::cerr << RED << "\nERROR : Connection failure" << RESET << std::endl;
		return ;
	}
	if (clientPfd.fd > 0)
		std::cerr << GREEN << "\n┌──────────\n│ New connection from: " \
			<< inet_ntoa(clientAddress.sin_addr) << "\n└──────────" << RESET << std::endl;
	clientPfd.events = POLLIN;
	clientPfd.revents = 0;
	fdList.push_back(clientPfd);
	fdMap[(fdList.end() - 1)->fd] = ntohs(serverPort.sin_port);
}

/*
	Deso Lea j'ai un peu retouche ca hihi
	start() va effectivement demarrer le serveur apres avoir setup tous les sockets et valeurs.
	On va populer un vecteur de pollfd qui contiendront tous les fd des sockets actifs.
	on rentre dans une boucle infinie qui fa bloquer sur poll() jusqu'a ce qu'il detecte de l'activite sur un socket.
	Si poll() fail, on va retenter MAX_ATTEMPTS avant de throw() une erreur.
	si paul() est cool avec nous, on va checker quel socket a de l'activite grace a POLLIN.
	Au moment ou on trouve le socket actif, on va lancer acceptConnection() avec le bon fd, qui prendra la releve.
*/

void	ConnectManager::start()
{
	std::vector<struct pollfd>	fds(0);
	std::vector<int>			readFds(0);
	std::map<int, ushort>		fdMap;
	int							pollResult;
	std::string					message;

	for (size_t i = 0; i < _serverFds.size(); i++)
	{
		struct pollfd serverPollFd;
		serverPollFd.fd = _serverFds[i];
		serverPollFd.events = POLLIN;
		serverPollFd.revents = 0;
		fds.push_back(serverPollFd);
	}
	_continue = false;
	while ((pollResult = poll(fds.data(), fds.size(), 10)) >= 0)
	{
		for (size_t i = 0; i < _serverFds.size(); i++)
			if (fds[i].revents & POLLIN)
				acceptConnection(_serverFds[i], fds, fdMap);
		for (size_t i = _serverFds.size(); i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN) {
				readMessage(fds[i].fd, &message);
				if (std::find(readFds.begin(), readFds.end(), fds[i].fd) == readFds.end()) {
					readFds.push_back(fds[i].fd);
				}
			}
			else {
				if (std::find(readFds.begin(), readFds.end(), fds[i].fd) != readFds.end()) {
					for (int n = 0; n < this->_serverList.getAmountOfServers(); n++)
					{
						const ServerConf&	currentSConf = _serverList.getServConf(n);
						if (std::find(currentSConf.getPort().begin(), currentSConf.getPort().end(), fdMap[fds[i].fd]) != currentSConf.getPort().end())
						{
							std::cout << "CLIENT " << fds[i].fd << " ON PORT " << fdMap[fds[i].fd] << " IS USING SERVER " << currentSConf.getServerName() << "\n";
							if (!(_continue = handleClient(fds[i], currentSConf, message)))
							{
								readFds.erase(std::find(readFds.begin(), readFds.end(), fds[i].fd));
								fds.erase(fds.begin() + i);
								--i;
							}
							message = "";
							break ;
						}
					}
				}
			}
		}
	}
	std::cerr << RED << "ERROR: poll() failure" << RESET << std::endl;
}
