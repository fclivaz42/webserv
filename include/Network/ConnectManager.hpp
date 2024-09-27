#ifndef CONNECTMANAGER_HPP
# define CONNECTMANAGER_HPP

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <utility>
#include <vector>
#include <sys/epoll.h>
#include <poll.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <string>
#include "webserv.hpp"
#include "Parsing/Servers.hpp"

class ConnectManager
{
	private:
		std::vector<int>										_serverFds;
		std::vector<struct sockaddr_in>							_serverPorts;
		std::vector<unsigned short>								_port;
		std::vector<std::pair<std::string, unsigned short> >	_pairPortsNames;
		const Servers&											_serverList;

		void	handleClient(int clientFd, const ServerConf& serverConf);
		void	closErase(size_t index);

	public:
		ConnectManager(const Servers& ServerList);
		~ConnectManager();

		//METHODS
		void				start();
		bool				isHttpRequest(const std::string& message);
		bool				startSocketListen(int backlog = SOMAXCONN);
		int					acceptConnection(int serverFd, std::vector<unsigned short>& portlist);
		std::string			readMessage(int clientFd);
		bool				isServerFd(int fd);
		bool				clientDeco(int fd);
		static std::string	readFile(const std::string& filePath);

		//GETTERS
		const std::vector<int>&	getServerFd() const;
};

#endif // CONNECTMANAGER_HPP
