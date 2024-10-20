/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fclivaz <fclivaz@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 22:01:35 by fclivaz           #+#    #+#             */
/*   Updated: 2024/10/21 00:19:27 by fclivaz          ###   LAUSANNE.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests/HTTPResponse.hpp"

std::string	HTTPResponse::generateResponse(unsigned int statusCode, const std::string& path, const std::string& alive, const ServerConf& sConf)
{
	std::string			contentType;
	std::string			content;
	std::string			errorPage;
	std::stringstream	errorStream;

	if (statusCode >= 400) {
		errorStream << sConf.getErrorPath() << statusCode << ".html";
		errorPage = errorStream.str();
		contentType = HTTPResponse::getContentType(errorPage);
		content = HTTPResponse::readFile(errorPage);
		std::cout << RED << "Error " << statusCode << " occured. " << RESET << "Sending page " << errorPage << std::endl;
	}
	else {
		contentType = HTTPResponse::getContentType(path);
		content = HTTPResponse::readFile(path);
	}

	switch (statusCode / 100)
	{
		case 2:
			switch (statusCode) {
				case 200:
					return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
				case 204:
					return ("HTTP/1.1 204 No Content\r\n" + alive + "\r\n");
			}
		case 3:
			switch (statusCode) {
				case 302:
					return ("HTTP/1.1 302 Found\r\nLocation: " + path + "\r\nConnection: close\r\n\r\n");
			}
		case 4:
			switch (statusCode)
			{
				case 400:
					throw HTTPResponse::ErrorCode("400 Bad Request", contentType, alive, content);
				case 403:
					throw HTTPResponse::ErrorCode("403 Forbidden", contentType, alive, content);
				case 404:
					throw HTTPResponse::ErrorCode("404 Not Found", contentType, alive, content);
				case 405:
					throw HTTPResponse::ErrorCode("405 Method Not Allowed", contentType + "\r\nAllow: " + path, alive, content);
				case 411:
					throw HTTPResponse::ErrorCode("411 Length Required", contentType, alive, content);
				case 413:
					throw HTTPResponse::ErrorCode("413 Content Too Large", contentType, alive, content);
				case 414:
					throw HTTPResponse::ErrorCode("414 URI Too Long", contentType, alive, content);
				case 415:
					throw HTTPResponse::ErrorCode("415 Unsupported Media Type", contentType, alive, content);
				case 417:
					throw HTTPResponse::ErrorCode("417 Expectation Failed", contentType, alive, content);
				case 418:
					throw HTTPResponse::ErrorCode("418 I'm a teapot", contentType, alive, content);
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
		case 5:
			switch (statusCode)
			{
				case 505:
					throw HTTPResponse::ErrorCode("505 HTTP Version Not Supported",contentType, alive, content);
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
		default:
			throw HTTPResponse::ISE(contentType, alive, content);

	}
}

bool	HTTPResponse::hasExtension(const std::string& path, const std::string& extension)
{
	size_t pos = path.rfind(extension);
	return ((pos != std::string::npos) && (pos == path.length() - extension.length()));
}

std::string	HTTPResponse::getContentType(const std::string& path)
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

std::string	HTTPResponse::readFile(const std::string& filePath)
{
	std::ifstream	file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw HTTPResponse::LightISE();
	std::stringstream	feur;
	feur << file.rdbuf();
	return (feur.str());
}
