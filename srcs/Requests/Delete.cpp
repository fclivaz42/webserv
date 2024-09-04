// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Delete.cpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/04 10:46:15 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:15:46 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Get.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processDeleteRequest(const HttpRequest& request)
{
	std::string path = request.getPath();
	std::cout << ORANGE << path << RESET << std::endl;

	path = updatePath(path);
	std::cout << path << std::endl;

	std::string fileContent = SocketManager::readFile("." + path);
	std::cout << PURPLE << "fileContent is: " << fileContent << RESET << std::endl; 

	std::string contentType = getMimeType(path);
	std::cout << PURPLE << "contentType is: " << contentType << RESET << std::endl; 

	std::string alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + fileContent);
}
