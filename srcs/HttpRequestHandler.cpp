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
#include "../include/SocketManager.hpp"

std::string	trim(const std::string& str)
{
	size_t	start = str.find_first_not_of(" \t");

	if (start == std::string::npos)
	{
		return ("");
	}
	size_t end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

std::string	HttpRequestHandler::handleRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string method, path, version;
	
	iss >> method >> path >> version;

	std::cout << RED << "Server received request: " << request << RESET << std::endl;
	if (path.find("file://") == 0)
		path.substr(7);

	if (method == "POST")
	{
		std::string 	headers;
		std::string 	body;
		bool		contentLengthFound = false;
		int		contentLength = 0;

		while (std::getline(iss, headers) && headers != "\r" && !headers.empty())
		{
			if (headers.find("Content-Length:") == 0)
			{
				contentLengthFound = true;
				std::string contentLengthStr = headers.substr(headers.find(":") + 1);
				contentLengthStr = trim(contentLengthStr);
				contentLength = std::atoi(contentLengthStr.c_str());
			}
		}
		if (contentLengthFound)
		{
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
		if (path == "/") path = "/pages/index.html";
		std::string fileContent = SocketManager::readFile("." + path);
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET request received");
	}
	if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETE request received");
	}
	return ("HTTP/1.1 405 Method not allowed\r\n\r\n");
}
