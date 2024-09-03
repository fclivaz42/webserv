// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpRequest.cpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/03 14:35:14 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/03 15:18:42 by lmedrano         ###   ########.fr       //
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
	
	std::getline(iss, line);
	std::istringstream requestLine(line);
	requestLine >> method >> path >> version;

	while (std::getline(iss, line) && line != "\r" && !line.empty())
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos)
		{
			std::string key = trim(line.substr(0, pos));
			std::string value = trim(line.substr(pos + 1));
			headers[key] = value;
		}
	}

	if (iss.peek() == '\r')
	{
		iss.ignore();
		if (iss.peek() == '\n')
			iss.ignore();
	}

	if (headers.find("Content-Length") != headers.end())
	{
		std::istringstream contentLenStream(headers["Content-Length"]);
		size_t contentLength = 0;
		contentLenStream >> contentLength;
		body.resize(contentLength);
		iss.read(&body[0], contentLength);
	}
}
