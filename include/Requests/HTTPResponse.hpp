/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fclivaz <fclivaz@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 22:01:40 by fclivaz           #+#    #+#             */
/*   Updated: 2024/10/21 20:34:44 by fclivaz          ###   LAUSANNE.ch       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Parsing/ServerConf.hpp"
#include "webserv.hpp"
#include <unistd.h>
#include <sstream>
#include <string>

class HTTPResponse
{
	private:
		static std::string	getContentType(const std::string& path);
		static std::string	readFile(const std::string& filePath);
		static bool			hasExtension(const std::string& path, const std::string& extension);


	public:
		static std::string	generateResponse(unsigned int statusCode,
												const std::string& path,
												const std::string& alive,
												const ServerConf& sConf);

		class ErrorCode : public std::exception {
			private:
				std::string	_str;
			public:
				ErrorCode(const std::string& code, const std::string& contentType, const std::string& alive, const std::string& content) {
					std::stringstream	contentStream;
					contentStream << content.length();
					this->_str = "HTTP/1.1 " + code + "\r\n" + "Content-Type: " + contentType + "\r\n" + alive + "Content-Length: " + contentStream.str() + "\r\n\r\n" + content;
				}
				~ErrorCode() throw () {}
				virtual char const	*what(void) const throw() {
					return _str.c_str();
				}
		};
		class ISE : public ErrorCode {
			public:
				ISE(const std::string& contentType, const std::string& alive, const std::string& content) :
					ErrorCode("500 Internal Server Error", contentType, alive, content) {}
		};
		class LightISE : public std::exception {
			public:
				virtual const char	*what() const throw() {
					return("HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n");
				}
		};
		class LightNotFound : public std::exception {
			public:
				virtual const char	*what() const throw() {
					return("HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
				}
		};
};

#endif
