// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Get.cpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:33:20 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 15:16:44 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Get.hpp"

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

std::string	processGetRequest(std::string path, std::string connectionHandler)
{
	std::cout << path << std::endl;
	path = "/public" + path;
	std::string fileContent = SocketManager::readFile("." + path);
	std::string contentType = getMimeType(path);
	return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + connectionHandler + "\r\n" + fileContent);
}
