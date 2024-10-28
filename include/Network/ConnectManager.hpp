#ifndef CONNECTMANAGER_HPP
# define CONNECTMANAGER_HPP

#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "webserv.hpp"
#include "Parsing/Servers.hpp"
#include "Requests/Get.hpp"
#include "Requests/Post.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"
#include "webserv.hpp"

#define MAX_ATTEMPTS 512

class ConnectManager
{
	private:
		std::vector<int>				_serverFds;
		std::vector<struct sockaddr_in>	_serverPorts;
		std::vector<ushort>				_port;
		const Servers&					_serverList;
		bool							_continue;

		ssize_t	readMessage(int clientFd, std::string *message);
		bool	handleClient(struct pollfd clientFd, const ServerConf& serverConf, std::string& message);
		void	acceptConnection(int serverFd, std::vector<struct pollfd>& fdList, std::map<int, ushort>& fdMap);
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
bool	isCGI(HTTPRequest &request);
#endif // CONNECTMANAGER_HPP
