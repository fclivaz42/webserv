#include "Requests/HTTPRequest.hpp"
#include <algorithm>
#include <ostream>
#include <string>

/*
	---------------------------------------------
	Constructors, Destructors and all that shizaz
	---------------------------------------------
*/

HTTPRequest::HTTPRequest() : _method(""), _path(""), _version(""), _body(std::string())
{}

HTTPRequest::HTTPRequest(std::stringstream& request, bool cont)
{
	std::string			line, key, value;
	size_t				pos;

	if (cont)
	{
		_method = "POST";
		_body << request.rdbuf();
		request.str(std::string());
		request.clear();
		return ;
	}

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
	requestLine >> _method >> _path >> _version;

	if (DEBUG) {
		std::cout << "├────────── REQUEST METADATA ──────────\n";
		std::cout << "│ Method: " << "" << _method << "" << std:: endl;
		std::cout << "│ Path: " << _path << std:: endl;
		std::cout << "│ Version: " << _version << "\n└────────── END REQUEST  ──────────\n";
	}
	//TODO throw real error response
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw HTTPRequest::RequestNotAllowed("GET, POST, DELETE");

	//TODO search url in location _path
	if (_path.empty() || _path[0] != '/')
		throw HTTPRequest::UnsupportedHTTPVersion();

	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		throw HTTPRequest::UnsupportedHTTPVersion();

	while (request.peek() != EOF)
	{
		std::getline(request, line);
		pos = line.find(":");
		if (pos != std::string::npos)
		{
			key = trim(line.substr(0, pos));
			value = trim(line.substr(pos + 1, line.length() - (pos + 1) - 1));
			if (key != "Content-Type")
				_headers[key] = value;
			else if (value == "application/x-www-form-urlencoded")
				_headers[key] = value;
			else if (value.find("multipart") != std::string::npos)
			{
				_headers["boundary"] = value.substr(value.find("boundary=") + 9);
				_headers[key] = value.substr(0, value.find(';'));
			}
		}
		else if (line == "\r")
			if (_headers["Content-Type"] == "application/x-www-form-urlencoded") {
				_body << request.rdbuf();
				break;
			}
			else
				continue;
		else if (line.find(_headers["boundary"]) != std::string::npos)
		{
			_body << request.rdbuf();
			break ;
		}
		else
			throw HTTPRequest::InvalidHeaders();
	}

	if (_version == "HTTP/1.1" && (_headers.find("Host") == _headers.end()))
		throw HTTPRequest::MissingHost();
	request.str(std::string());
	request.clear();
}

HTTPRequest::HTTPRequest(HTTPRequest const &copy) :	_method(copy._method),
													_path(copy._path),
													_version(copy._version),
													_headers(copy._headers)
{
	_body.str(copy._body.str());
}

HTTPRequest::~HTTPRequest()
{}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &rhs)
{
	if (this != &rhs)
	{
		_method = rhs._method;
		_path = rhs._path;
		_version = rhs._version;
		_headers = rhs._headers;
		_body.str(rhs._body.str());
	}
	return (*this);
}

/*
	---------------------------------------------
			Getters because I love OOP
	---------------------------------------------
*/

const std::string&	HTTPRequest::getMethod() const
{
	return (_method);
}

const std::string&	HTTPRequest::getPath() const
{
	return (_path);
}

const std::string&	HTTPRequest::getVersion() const
{
	return (_version);
}

const std::map<std::string, std::string>&	HTTPRequest::getHeaders() const
{
	return (_headers);
}

std::stringstream&	HTTPRequest::getBody()
{
	return (_body);
}

const std::string	HTTPRequest::isKeepAlive() const
{
	std::map<std::string, std::string>::const_iterator iter = _headers.find("Connection");

	if (iter != _headers.end())
	{
		std::string alive  = iter->second;
		std::transform(alive.begin(), alive.end(), alive.begin(), ::tolower);
		return ("Connection: " + alive + "\r\n");
	}
	return (_version == "HTTP/1.1") ? "Connection: keep-alive\r\n" : "Connection: close\r\n";
}

/*
	---------------------------------------------
				Extra functions teehee
	---------------------------------------------
*/

std::string	trim(const std::string& str)
{
	size_t	start = str.find_first_not_of(" \t");

	if (start == std::string::npos)
		return ("");
	size_t end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}
