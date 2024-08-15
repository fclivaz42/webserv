// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   SocketManager.hpp                                  :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 11:07:53 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 11:20:08 by lmedrano         ###   ########.fr       //
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


#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"

class SocketManager
{
	private:
		int 			_serverFd;
		struct sockaddr_in	_serverAddress;
	public:
		SocketManager();
		~SocketManager();
		bool	createSocket();
		bool	bindSocket(int port);
		bool	startListening(int backlog = 10);
		int	acceptConnection();
		int	getServerFd() const;
};

#endif
