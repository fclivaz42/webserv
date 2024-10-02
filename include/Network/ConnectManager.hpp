#ifndef CONNECTMANAGER_HPP
# define CONNECTMANAGER_HPP

#include <map>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <poll.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <string>
#include "webserv.hpp"
#include "Parsing/Servers.hpp"

#define MAX_ATTEMPTS 512

class ConnectManager
{
	private:
		std::vector<int>				_serverFds;
		std::vector<struct sockaddr_in>	_serverPorts;
		std::vector<ushort>				_port;
		const Servers&					_serverList;

		ssize_t	readMessage(int clientFd, std::stringstream& message);
		void	handleClient(struct pollfd clientFd, const ServerConf& serverConf, std::stringstream& message);
		void	acceptConnection(int serverFd, std::vector<struct pollfd>& fdList, std::map<int, ushort>& swag);
		void	closErase(size_t index);

	public:
		ConnectManager(const Servers& ServerList);
		~ConnectManager();

		//METHODS
		void				start();
		bool				startSocketListen(int backlog = SOMAXCONN);
		static std::string	readFile(const std::string& filePath);

		class TooManyFailures : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "poll() failed too many times. Exiting.\n";
				}
		};
};

#endif // CONNECTMANAGER_HPP
