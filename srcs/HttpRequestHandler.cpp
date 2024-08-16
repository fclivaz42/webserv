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

	std::cout << RED << "Server received request: " << request << RESET << std::endl;
	if (method == "POST")
	{
		std::string headers;
		std::string body;
		while (std::getline(iss, headers) && headers != "\r")
		{
			// do smth with headers
		}
		if (headers.find("Content-Length:") != std::string::npos)
		{
			std::string trim = headers.substr(headers.find(":") + 1);
			int	contentLength = std::atoi(trim.c_str());
			body.resize(contentLength);
			iss.read(&body[0], contentLength);
			if (iss.gcount() < contentLength)
			{
				return ("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nIncomplete request body");
			}
		}
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST request received with body");
	}

	if (method == "GET")
	{
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET request received");
	}
	if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETE request received");
	}
	return ("HTTP/1.1 405 Method not allowed\r\n\r\n");
}
