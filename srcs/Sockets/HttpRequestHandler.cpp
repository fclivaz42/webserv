// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 14:08:14 by lmedrano          #+#    #+#             //
/*   Updated: 2024/08/29 21:00:10 by fclivaz          ###   LAUSANNE.ch       */
//                                                                            //
// ************************************************************************** //

#include "Sockets/HttpRequestHandler.hpp"
#include "Sockets/SocketManager.hpp"

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

bool		hasExtension(const std::string& path, const std::string& extension)
{
	std::string::size_type pos = path.rfind(extension);
	return ((pos != std::string::npos) && (pos == path.length() - extension.length()));
}

std::string	getMimeType(const std::string& path)
{
	if (hasExtension(path, ".css"))
		return ("text/css");
	if (hasExtension(path, ".html"))
		return ("text/html");
	if (hasExtension(path, ".js"))
		return ("application/javascript");
	if (hasExtension(path, ".png"))
		return ("image/png");
	if (hasExtension(path, ".jpg") || hasExtension(path, ".jpeg"))
		return ("image/jpeg");
	if (hasExtension(path, ".gif"))
		return ("image/gif");
	return ("application/octet-stream");
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
		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nPOST request received with body");
	}

	if (method == "GET")
	{
		if (path == "/")
			path = "/public/index.html";
		else
			path = "/public" + path;
		std::string fileContent = SocketManager::readFile("." + path);
		std::string contentType = getMimeType(path);
		return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + connectionHandler + "\r\n" + fileContent);
	}
	if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nDELETE request received");
	}
	return ("HTTP/1.1 405 Method not allowed\r\n\r\n" + connectionHandler + "\r\n");
}
