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

		ssize_t				readMessage(int clientFd, std::string& message);
		void				acceptConnection(int serverFd, std::vector<struct pollfd>& fdList, std::map<int, std::string>& fdMap);
		void				closErase(size_t index);
		void				initializeRequest(int clientFd, const std::string& message, std::map<int, HTTPRequest*>& fdRequestMap);
		void				writeToClient(const std::string& response, int clientFd);
		void				redirectPath(const ServerConf& sConf,const Location& loc, std::string& path);
		const std::string	handleClient(HTTPRequest& request);
		const ServerConf&	findSconfFromHost(std::map<std::string, std::string>& headers);
		const Location&		findLocationFromSConf(const ServerConf& sConf, const std::string& path);
		const std::string	createPath(const HTTPRequest* req) const;

	public:
		ConnectManager(const Servers& ServerList);
		~ConnectManager();

		//METHODS
		void				start();
		bool				startSocketListen(int backlog = SOMAXCONN);

		class TooManyFailures : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "poll() failed too many times. Exiting.\n";
				}
		};
};

#endif // CONNECTMANAGER_HPP
