// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpResponse.cpp                                   :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/07 18:24:02 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/07 18:50:02 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/HttpResponse.hpp"
#include "Requests/Get.hpp"

HttpResponse::HttpResponse(const ServerConf& serverConf) : serverConf(serverConf)
{}

std::string		HttpResponse::generateResponse(const std::string& statusCode, const std::string& path) const
{
	if (statusCode == "404")
	{
		std::string content = SocketManager::readFile(path);
		std::cout << RED << content << RESET << std::endl;
		std::string contentType = getContentType(path);
		std::cout << RED << contentType << RESET << std::endl;
		return ("HTTP/1.1 404 Not Found\r\nContent-Type: " + contentType +  "\r\n\r\n" + content);
	}
	else if (statusCode == "403")
	{
		std::string content = SocketManager::readFile(path);
		std::string contentType = getContentType(path);
		return ("HTTP/1.1 403 Forbidden\r\n" + getErrorPage("403") + "\r\n");
	}
	else if (statusCode == "200")
	{
		std::string content = SocketManager::readFile(path);
		std::string contentType = getContentType(path);
		return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n\r\n" + content);
	}
	return ("HTTP/1.1 500 Internal Server Error\r\n\r\n");
}

std::string		HttpResponse::getErrorPage(const std::string& errorCode) const
{
	std::string	errorPath;
	(void) errorCode;
	//TODO create this muddafucka
	//errorPath = serverConf.getErrorPage(errorCode);
	if (errorPath.empty())
		errorPath = "/public/errors/404.html";
	return (readFileContent(errorPath));
}

std::string		HttpResponse::getContentType(const std::string& path) const
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

std::string		HttpResponse::readFileContent(const std::string& path) const
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cerr << "ERROR: Unable to open file: " << path << std::endl;
		return ("");
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	return (oss.str());
}

