// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpRequest.cpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/03 14:35:14 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:31:23 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/HttpRequest.hpp"

HttpRequest::HttpRequest() : method(""), path(""), version(""), body("")
{}

HttpRequest::HttpRequest(const std::string& request)
{
	parseRequest(request);
}

/* COPY CONSTRUCTOR */
HttpRequest::HttpRequest(HttpRequest const &copy) : method(copy.method), path(copy.path), version(copy.version), headers(copy.headers), body(copy.body)
{}

/* DESTRUCTOR */
HttpRequest::~HttpRequest()
{}

/* SURCHARGED OPERATORS */
HttpRequest &HttpRequest::operator=(HttpRequest const &rhs)
{
	if (this != &rhs)
	{
		method = rhs.method;
		path = rhs.path;
		version = rhs.version;
		headers = rhs.headers;
		body = rhs.body;
	}
	return (*this);
}

std::string				HttpRequest::getMethod() const
{
	return (method);
}

std::string				HttpRequest::getPath() const
{
	return (path);
}

std::string				HttpRequest::getVersion() const
{
	return (version);
}

std::map<std::string, std::string>	HttpRequest::getHeaders() const
{
	return (headers);
}

std::string				HttpRequest::getBody() const
{
	return (body);
}

std::string	trim(const std::string& str)
{
	size_t	start = str.find_first_not_of(" \t");

	if (start == std::string::npos)
		return ("");
	size_t end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

void	HttpRequest::parseRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string		line;
	
	std::cout << request<< std::endl;
	if (!std::getline(iss, line) || line.empty())
	{
		std::cerr << ORANGE << "Waiting for request . . ." << RESET << std::endl;
		return ;
	}
	std::istringstream requestLine(line);
	requestLine >> method >> path >> version;

	std::cout << "method: " << "" << method << "" << std:: endl;
	std::cout << "path: " << path << std:: endl;
	std::cout << "version: " << version << std:: endl;
	//TODO throw real error response
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw std::runtime_error("ERROR: Request not allowed");

	//TODO search url in location path
	if (path.empty() || path[0] != '/')
		throw std::runtime_error("ERROR: Invalid path");

	if (version != "HTTP/1.1" && version != "HTTP/1.0")
		throw std::runtime_error("ERROR: Unsupported HTTP version");

	while (std::getline(iss, line) && line != "\r" && !line.empty())
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos)
		{
			std::string key = trim(line.substr(0, pos));
			std::string value = trim(line.substr(pos + 1));
			headers[key] = value;
		}
		else
			throw std::runtime_error("ERROR: Invalid headers");
	}


	if (version == "HTTP/1.1" && headers.find("Host") == headers.end())
		throw std::runtime_error("ERROR: Missing host");

	if (headers.find("Content-Length") != headers.end())
	{
		std::istringstream contentLenStream(headers["Content-Length"]);
		size_t contentLength = 0;
		contentLenStream >> contentLength;
		body.resize(contentLength);
		iss.read(&body[0], contentLength);
	}
}

bool	HttpRequest::isKeepAlive() const
{
	std::map<std::string, std::string>::const_iterator iter = headers.find("Connection");

	if (iter != headers.end())
	{
		std::string alive  = iter->second;
		std::transform(alive.begin(), alive.end(), alive.begin(), ::tolower);
		if (alive == "keep-alive")
			return (true);
		if (alive == "close")
			return (false);
	}
	if (version == "HTTP/1.1")
		return (true);
	return (false);

}
