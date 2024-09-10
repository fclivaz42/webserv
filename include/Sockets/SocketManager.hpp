#ifndef SOCKETMANAGER_HPP
# define SOCKETMANAGER_HPP

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <poll.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <string>
#include "webserv.hpp"
#include "Parsing/ServerConf.hpp"

class SocketManager
{
	private:
		std::vector<int>				_serverFd;
		std::vector<struct sockaddr_in>	_serverAddress;
		std::vector<unsigned short>		_port;
		std::string						_host;
		
		void			handleClient(int clientFd, const ServerConf& serverConf);
	public:
		SocketManager(const ServerConf& serverConf);
		~SocketManager();

		//METHODS
		bool			isHttpRequest(const std::string& message);
		bool			createSocket();
		bool			bindSocket();
		bool			startListening(int backlog = 10);
		int				acceptConnection(int serverFd);
		std::string		readMessage(int clientFd);
		int				start(const ServerConf& serverConf);
		bool			isServerFd(int fd);
		bool			clientDeco(int fd);
		static std::string	readFile(const std::string& filePath);
		//GETTERS
		const std::vector<int>&	getServerFd() const;
		//SETTERS
		void			setPorts(std::vector<unsigned short>& port);
		void			setHost(const std::string& host);
};

#endif
