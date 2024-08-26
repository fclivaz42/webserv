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

//FONCTION THAT PROCESSES AN HTTP REQUEST AND GENERATES A RESPONSE
//Je cree un input stream de la request avec ifstream
//J'extraie la http method, le path de la request la http version et la request string
//Je print la request recue
//Je check le path
//je handle si POST/GET/DELETE ou autre
//Je checke quelle reuqte j'ai
//Je checke que j'ai bien le bon path 
//je lis le contenu de la request 
//je renvoie le bon status code
std::string	HttpRequestHandler::handleRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string method, path, version;
	std::string headers;
	bool keepAlive = false;
	
	iss >> method >> path >> version;

//	std::cout << RED << "Server received request: " << request << RESET << std::endl;
	if (path.find("file://") == 0)
		path.substr(7);

	while (std::getline(iss, headers) && headers != "\r" && !headers.empty())
	{
		if (headers.find("Connection:") == 0)
		{
			std::string coType = headers.substr(headers.find(":") + 1);
			coType = trim(coType);
			if (coType == "keep-alive")
			{
				keepAlive = true;
			}
		}
	}

	std::string connectionHandler = keepAlive ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

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
				return ("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nIncomplete request body");
			} 
		}
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nPOST request received with body");
	}

	if (method == "GET")
	{
		std::cout << "path: " << path << std::endl;
		if (path == "/") path = "/public/index.html";
		std::string fileContent = SocketManager::readFile("." + path);
		std::cout << "file content: " << fileContent << std::endl;
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + fileContent);
	}
	if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nDELETE request received");
	}
	return ("HTTP/1.1 405 Method not allowed\r\n\r\n" + connectionHandler + "\r\n");
}
