// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpRequestHandler.cpp                             :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 14:08:14 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 14:20:51 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "../include/HttpRequestHandler.hpp"

std::string	HttpRequestHandler::handleRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string method, path, version;
	
	iss >> method >> path >> version;

	if (method == "GET")
	{
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET request received");
	}
	else if (method == "POST")
	{
		std::string body;
		std::getline(iss, body);
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST request received with body");
	}
	else if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETE request received");
	}

	else
	{
		return ("HTTP/1.1 405 Method not allowed\r\n\r\n");
	}
}
