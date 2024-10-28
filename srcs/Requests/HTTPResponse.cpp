/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fclivaz <fclivaz@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 22:01:35 by fclivaz           #+#    #+#             */
/*   Updated: 2024/10/27 20:39:02 by fclivaz          ###   LAUSANNE.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests/HTTPResponse.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Network/ConnectManager.hpp"

std::string	HTTPResponse::listDirectory(const std::string& path, const std::string& refPath)
{
	std::string		html = HEADERS;
	struct dirent	*dent;
	DIR				*lst = opendir(path.c_str());

	if (lst == NULL)
		return "\t<h1>Error opening requested path.</h1>\n</body>";
	html.replace(html.find("++PATH++"), 8, refPath);
	html += "\t<h1>Index of " + refPath + (*refPath.end() - 1 == '/' ? "" : "/") + "</h1>\n<hr width='100%' size='2' color='black'>\n";
	while ((dent = readdir(lst)) != NULL)
		html += "\t<li><a href='" + refPath + (*refPath.end() - 1 == '/' ? "" : "/") + static_cast<std::string>(dent->d_name) + "'>" +
			static_cast<std::string>(dent->d_name) + "</a></li>\n";
	html += "<hr width='100%' size='2' color='black'>\n\t<p>webserv</p>\n</body>\n</html>";
	return html;
}

std::string	HTTPResponse::generateResponse(unsigned int statusCode, const std::string& path, const std::string& alive, HTTPRequest& request)
{
	struct stat			s;
	std::string			contentType;
	std::string			content;
	std::string			errorPage;
	std::stringstream	errorStream;

	if (statusCode >= 400) {
		errorStream << request.getSConf().getErrorPath() << statusCode << ".html";
		errorPage = errorStream.str();
		std::cout << RED << "Error " << statusCode << " occured. " << RESET << "Sending page " << errorPage << std::endl;
		contentType = HTTPResponse::getContentType(errorPage);
		content = HTTPResponse::readFile(errorPage);
	}
	else if (statusCode == 200){
		contentType = request.getContent("Content-Type");
		content = HTTPResponse::readFile(request.getCreatedPath());
		std::cout << "CONTENTTUPE : " << contentType << std::endl << "Content  : " << content << std::endl;
	}
	else if (statusCode >= 200 && statusCode != 204 && statusCode < 300) {
		stat(path.c_str(), &s);
		if (s.st_mode & S_IFDIR) {
			contentType = "text/html";
			content = HTTPResponse::listDirectory(path, request.getPath());
		}
		else {
			contentType = HTTPResponse::getContentType(path);
			content = HTTPResponse::readFile(path);
		}
	}

	switch (statusCode / 100)
	{
		case 1:
			switch (statusCode) {
				case 100:
					return("HTTP/1.1 100 Continue\r\n" + alive + "Content-Length: 0\r\n\r\n");
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
			break ;
		case 2:
			switch (statusCode) {
				case 200:
					return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
				case 201:
					return ("HTTP/1.1 201 Created\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
				case 204:
					return ("HTTP/1.1 204 No Content\r\n" + alive + "\r\n");
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
			break ;
		case 3:
			switch (statusCode) {
				case 302:
					throw HTTPResponse::ErrorCode("302 Found", "text/html\r\nLocation: " + path, alive, "");
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
			break ;
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
			break ;
		case 5:
			switch (statusCode)
			{
				case 505:
					throw HTTPResponse::ErrorCode("505 HTTP Version Not Supported",contentType, alive, content);
				default:
					throw HTTPResponse::ISE(contentType, alive, content);
			}
			break ;
		default:
			throw HTTPResponse::ISE(contentType, alive, content);

	}
	return("HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n");
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
	if (access(filePath.c_str(), F_OK) != 0) {
		std::cout << ORANGE << "WARNING: Could not find " << RESET << filePath << "\n";
		return "";
	}

	std::ifstream	file(filePath.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cout << ORANGE << "WARNING: Could not open " << RESET << filePath << "\n";
		return "";
	}
	std::stringstream	feur;
	feur << file.rdbuf();
	return (feur.str());
}
