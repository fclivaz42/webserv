// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpRequestHandler.cpp                             :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 14:08:14 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 16:20:15 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Sockets/HttpRequestHandler.hpp"
#include "Requests/Get.hpp"
#include "Requests/Post.hpp"

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
	std::string method, path, version, body;
	std::string headers;
	bool keepAlive = false;
	size_t contentLength = 0;
	
	std::getline(iss, headers);
	std::istringstream requestLine(headers);
	requestLine >> method >> path >> version;

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
		else if (headers.find("Content-Length:") == 0)
		{
			std::string cl = headers.substr(headers.find(":") + 1);
			cl = trim(cl);
			contentLength = static_cast<size_t>(atoi(cl.c_str()));
		}
	}

	if (iss.peek() == '\r')
	{
		iss.ignore();
		if (iss.peek() == '\n')
			iss.ignore();
	}

	std::string connectionHandler = keepAlive ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	if (method == "GET")
	{
		return (processGetRequest(path, keepAlive ? "Connection: keep-alive\r\n" : "Connection: close\r\n"));
	}
	if (method == "POST" && contentLength > 0)
	{
		body.resize(contentLength);
		iss.read(&body[0], contentLength);

		return (processPostRequest(body, keepAlive ? "Connection: keep-alive\r\n" : "Connection: close\r\n"));
	}
	if (method == "DELETE")
	{

		return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\nDELETE request received");
	}
	return ("HTTP/1.1 405 Method not allowed\r\n\r\n" + connectionHandler + "\r\n");
}
