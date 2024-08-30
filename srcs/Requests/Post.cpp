// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:39:31 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 15:47:56 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Post.hpp"

std::string urlDecode(const std::string& str) {
    std::string result;
    size_t length = str.length();
    for (size_t i = 0; i < length; ++i) {
        if (str[i] == '%') {
            if (i + 2 < length) {
                int value;
                std::istringstream is(str.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    result += static_cast<char>(value);
                    i += 2;
                }
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}


std::string	processPostRequest(std::string body, std::string connectionHandler)
{
	std::map<std::string, std::string> formData;
	std::istringstream bodyStream(body);
	std::string keyValue;

	while (std::getline(bodyStream, keyValue, '&'))
	{
		size_t pos = keyValue.find('=');
		if (pos != std::string::npos)
		{
			std::string key = urlDecode(keyValue.substr(0, pos));
			std::string value = urlDecode(keyValue.substr(pos + 1));
			formData[key] = value;
		}
	}

	std::string username = formData["name"];
	std::string email = formData["email"];
	std::string message = formData["message"];
	std::string response = "POST request received with data:\nUsername: " + username + "\nEmail: " + email + "\nMessage: " + message;
	return ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n" + connectionHandler + "\r\n" + response + "\r\n");
}
