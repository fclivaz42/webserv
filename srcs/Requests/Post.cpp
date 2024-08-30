// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:39:31 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 15:15:12 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Post.hpp"

std::string	processPostRequest(std::string body, std::istringstream iss, size_t contentLength, std::string connectionHandler)
{
	body.resize(contentLength);
	iss.read(&body[0], contentLength);

	std::map<std::string, std::string> formData;
	std::istringstream bodyStream(body);
	std::string keyValue;

	while (std::getline(bodyStream, keyValue, '&'))
	{
		size_t pos = keyValue.find('=');
		if (pos != std::string::npos)
		{
			std::string key = keyValue.substr(0, pos);
			std::string value = keyValue.substr(pos + 1);
			formData[key] = value;
		}
	}

	std::string username = formData["username"];
	std::string email = formData["email"];
	std::string response = "POST request received with data:\nUsername: " + username + "\nEmail: " + email;
	return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\n" + response + "\r\n");
}
