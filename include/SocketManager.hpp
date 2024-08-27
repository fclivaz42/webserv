// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   SocketManager.hpp                                  :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 11:07:53 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 14:40:55 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

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


#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"

#define BUFFER_SIZE 1024

class SocketManager
{
	private:
		std::vector<int>		_serverFd;
		std::vector<struct sockaddr_in>	_serverAddress;
		std::vector<int>		_port;
		std::string			_host;
		
		void			handleClient(int clientFd);
	public:
		SocketManager(const std::vector<int>& port);
		~SocketManager();

		//METHODS
		bool			isHttpRequest(const std::string& message);
		bool			createSocket();
		bool			bindSocket();
		bool			startListening(int backlog = 10);
		int				acceptConnection(int serverFd);
		std::string		readMessage(int clientFd);
		int				start();
		bool			isServerFd(int fd);
		bool			clientDeco(int fd);
		static std::string	readFile(const std::string& filePath);
		//GETTERS
		const std::vector<int>&	getServerFd() const;
		//SETTERS
		void			setPorts(std::vector <int>& port);
		void			setHost(const std::string& host);
};

#endif
