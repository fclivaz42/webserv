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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>
#include <poll.h>
#include <cstdlib>


#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"

#define BUFFER_SIZE 1024

class SocketManager
{
	private:
		int 			_serverFd;
		struct sockaddr_in	_serverAddress;
		int			_port;
		std::string		_host;
		
		void			handleClient(int clientFd);
	public:
		SocketManager();
		~SocketManager();

		//METHODS
		bool		isHttpRequest(const std::string& message);
		bool		createSocket();
		bool		bindSocket();
		bool		startListening(int backlog = 10);
		int		acceptConnection();
		std::string	readMessage(int clientFd);
		int		start();

		//GETTERS
		int	getServerFd() const;

		//SETTERS
		void	setPort(int port);
		void	setHost(const std::string& host);
};

#endif
