#include "Requests/HttpRequest.hpp"
#include <algorithm>
#include <ostream>
#include <string>

HttpRequest::HttpRequest() : method(""), path(""), version(""), body("")
{}

HttpRequest::HttpRequest(std::stringstream& request)
{
	std::string			line, key, value;
	size_t				pos;

	if (DEBUG) {
		std::stringstream	stRequest(request.str());
		std::string			prRequest;
		std::cout << "\n┌────────── NEW REQUEST ──────────\n";
		while (std::getline(stRequest, prRequest) && prRequest.find("Content-Disposition") == std::string::npos)
			if (stRequest.peek() != EOF)
				std::cout << "│ " << prRequest << std::endl;
		stRequest.str(std::string());
		stRequest.clear();
	}
	if (!std::getline(request, line) || line.empty())
	{
		std::cerr << ORANGE << "Waiting for request . . ." << RESET << std::endl;
		return ;
	}
	std::istringstream requestLine(line);
	requestLine >> method >> path >> version;

	if (DEBUG) {
		std::cout << "├────────── REQUEST METADATA ──────────\n";
		std::cout << "│ method: " << "" << method << "" << std:: endl;
		std::cout << "│ path: " << path << std:: endl;
		std::cout << "│ version: " << version << "\n└────────── END REQUEST  ──────────\n";
	}
	//TODO throw real error response
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw std::runtime_error("ERROR: Request not allowed\n");

	//TODO search url in location path
	if (path.empty() || path[0] != '/')
		throw std::runtime_error("ERROR: Invalid path\n");

	if (version != "HTTP/1.1" && version != "HTTP/1.0")
		throw std::runtime_error("ERROR: Unsupported HTTP version\n");

	while (request.peek() != EOF)
	{
		std::getline(request, line);
		pos = line.find(":");
		if (pos != std::string::npos)
		{
			key = trim(line.substr(0, pos));
			value = trim(line.substr(pos + 1, line.length() - (pos + 1) - 1));
			if (key != "Content-Type")
				headers[key] = value;
			else if (value == "application/x-www-form-urlencoded")
				headers[key] = value;
			else if (value.find("multipart") != std::string::npos)
			{
				headers["boundary"] = value.substr(value.find("boundary=") + 9);
				headers[key] = value.substr(0, value.find(';'));
			}
		}
		else if (line == "\r")
			if (headers["Content-Type"] == "application/x-www-form-urlencoded") {
				std::getline(request, body);
				break;
			}
			else
				continue;
		else if (line.find(headers["boundary"]) != std::string::npos)
		{
			while (request.peek() != EOF)
			{
				std::getline(request, line);
				body.append(line).append("\n");
			}
			break ;
		}
		else
			throw std::runtime_error("ERROR: Invalid headers\n");
	}

	if (version == "HTTP/1.1" && headers.find("Host") == headers.end())
		throw std::runtime_error("ERROR: Missing host\n");
	request.str(std::string());
	request.clear();
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

const std::string&	HttpRequest::getMethod() const
{
	return (method);
}

const std::string&	HttpRequest::getPath() const
{
	return (path);
}

const std::string&	HttpRequest::getVersion() const
{
	return (version);
}

const std::map<std::string, std::string>&	HttpRequest::getHeaders() const
{
	return (headers);
}

const std::string&	HttpRequest::getBody() const
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
