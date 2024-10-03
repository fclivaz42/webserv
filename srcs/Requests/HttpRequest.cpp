#include "Requests/HttpRequest.hpp"
#include <algorithm>
#include <ostream>
#include <string>

/*
	---------------------------------------------
	Constructors, Destructors and all that shizaz
	---------------------------------------------
*/

HttpRequest::HttpRequest() : _method(""), _path(""), _version(""), _body(std::string())
{}

HttpRequest::HttpRequest(std::stringstream& request, bool cont)
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
		throw std::runtime_error("ERROR: Request not allowed\n");

	//TODO search url in location _path
	if (_path.empty() || _path[0] != '/')
		throw std::runtime_error("ERROR: Invalid path\n");

	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
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
			throw std::runtime_error("ERROR: Invalid headers\n");
	}

	if (_version == "HTTP/1.1" && _headers.find("Host") == _headers.end())
		throw std::runtime_error("ERROR: Missing host\n");
	request.str(std::string());
	request.clear();
}

HttpRequest::HttpRequest(HttpRequest const &copy) : _method(copy._method), _path(copy._path), _version(copy._version), _headers(copy._headers)
{
	_body.str(copy._body.str());
}

HttpRequest::~HttpRequest()
{}

HttpRequest &HttpRequest::operator=(HttpRequest const &rhs)
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

const std::string&	HttpRequest::getMethod() const
{
	return (_method);
}

const std::string&	HttpRequest::getPath() const
{
	return (_path);
}

const std::string&	HttpRequest::getVersion() const
{
	return (_version);
}

const std::map<std::string, std::string>&	HttpRequest::getHeaders() const
{
	return (_headers);
}

std::stringstream&	HttpRequest::getBody()
{
	return (_body);
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

bool	HttpRequest::isKeepAlive() const
{
	std::map<std::string, std::string>::const_iterator iter = _headers.find("Connection");

	if (iter != _headers.end())
	{
		std::string alive  = iter->second;
		std::transform(alive.begin(), alive.end(), alive.begin(), ::tolower);
		if (alive == "keep-alive")
			return (true);
		if (alive == "close")
			return (false);
	}
	if (_version == "HTTP/1.1")
		return (true);
	return (false);

}
