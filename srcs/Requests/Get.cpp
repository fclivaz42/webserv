// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Get.cpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:33:20 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:31:53 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Get.hpp"
#include "Requests/HttpRequest.hpp"

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

std::string updatePath(const std::string &path)
{
	//TODO do something to manage favicon ?
	if (path == "/favicon.ico")
		return (path);
	else if (path == "/")
		return ("/public/index.html");
	else if (path.find("public") == std::string::npos)
		return ("/public" + path);
	else
		return ("404 not found");
	return (path);
}

std::string	processGetRequest(const HttpRequest& request)
{
	std::string path = request.getPath();
	std::cout << ORANGE << path << RESET << std::endl;

	path = updatePath(path);
	std::cout << GREEN << path << RESET << std::endl;

	std::string fileContent = SocketManager::readFile("." + path);
	std::cout << PURPLE << "fileContent is: " << fileContent << RESET << std::endl; 

	std::string contentType = getMimeType(path);
	std::cout << PURPLE << "contentType is: " << contentType << RESET << std::endl; 

	std::string alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + fileContent);
}
