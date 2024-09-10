// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:39:31 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:19:07 by lmedrano         ###   ########.fr       //
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


std::string	processPostRequest(const HttpRequest& request)
{
	std::map<std::string, std::string> formData;
	std::istringstream bodyStream(request.getBody());
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

	std::string	username = formData["name"];
	std::string	email = formData["email"];
	std::string	message = formData["message"];
	std::string	alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	std::string htmlRes = 
	"<!doctype html>"
	"<html lang=\"en\">"
	"<head>"
	"<meta charset=\"utf-8\">"
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
	"<title>Contact Form</title>"
	"<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"/>"
	"</head>"
	"<body>"
	"<nav>"
	"<a class=\"request_button\" href=\"/formulaire.html\">Get in Touch !</a>"
	"<a class=\"black request_button\" href=\"/index.html\">Welcome Page</a>"
	"<a class=\"request_button\" href=\"/upload.html\">Upload a picture</a>"
	"</nav>"
	"<h1>Form successfully submitted!</h1>"
	"<p>Thank you for your submission " + username + ".</p>"
	"<p>Your email is: " + email + "</p>"
	"<p>Your message is: " + message + "</p>"
	"</br>"
	"</br>"
	"</br>"
    	"<p><a class=\"request_button\" href=\"/index.html\">Return to Home</a></p>"
	"<p></p>"
	"</body>"
	"</html>";

	std::string response = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n" +
	alive + "\r\n" +
	htmlRes + "\r\n";

	return (response);
}
