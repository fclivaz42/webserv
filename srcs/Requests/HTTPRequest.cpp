#include "Requests/HTTPRequest.hpp"

/*
	---------------------------------------------
	Constructors, Destructors and all that shizaz
	---------------------------------------------
*/

HTTPRequest::HTTPRequest() : _method(""), _path(""), _version(""), _body(std::string()), _bodySize(0)
{}

HTTPRequest::HTTPRequest(std::stringstream& request, const ServerConf& sConf, bool cont) :_method(""), _path(""), _version(""), _body(std::string()), _bodySize(0)
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
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		HTTPResponse::generateResponse(405, "GET, POST, DELETE", this->isKeepAlive(), sConf);

	//TODO: search url in location _path
	if (_path.empty() || _path[0] != '/')
		HTTPResponse::generateResponse(400, "", this->isKeepAlive(), sConf);

	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		HTTPResponse::generateResponse(505, "", this->isKeepAlive(), sConf);

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
			HTTPResponse::generateResponse(400, "", this->isKeepAlive(), sConf);
	}

	if (_version == "HTTP/1.1" && (_headers.find("Host") == _headers.end()))
		HTTPResponse::generateResponse(400, "", this->isKeepAlive(), sConf);

	if (_method == "POST")
	{
		if (_headers.find("Content-Length") == _headers.end())
			HTTPResponse::generateResponse(411, "", this->isKeepAlive(), sConf);
		if (_headers.find("Content-Type") == _headers.end())
			HTTPResponse::generateResponse(415, "", this->isKeepAlive(), sConf);

		char	*ptr;
		long	testsize = strtol(_headers["Content-Length"].c_str(), &ptr, 10);

		if (testsize < 0 || ptr[0] != 0)
			HTTPResponse::generateResponse(400, "", this->isKeepAlive(), sConf);

		_bodySize = strtoul(_headers["Content-Length"].c_str(), NULL, 10);
	}
	request.str(std::string());
	request.clear();
}

HTTPRequest::HTTPRequest(HTTPRequest const &copy) :	_method(copy._method),
													_path(copy._path),
													_version(copy._version),
													_headers(copy._headers),
													_bodySize(copy._bodySize)
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
		_bodySize= rhs._bodySize;
	}
	return (*this);
}

const std::string	HTTPRequest::createPath(const std::string& path, const ServerConf& sConf, const std::string& method, const std::string& attrib)
{
	std::map<std::string, Location>	locationMap = sConf.getLocation();
	std::string						returnPath, locReq, allowedMethods;
	struct stat						s;
	Location						loc;
	bool							allowedMethod = false;
	size_t							pos;

	pos = path.find_last_of('/');
	if (pos == 0)
		locReq = "/";
	else if (path.find(".") != std::string::npos)
		locReq = path.substr(0, pos);
	else if (path[path.length() - 1] == '/')
		locReq = path.substr(0, path.length() - 1);
	else
		locReq = path;

	std::cout << "LOCREQ " << locReq << std::endl;

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++)
		if (!locReq.compare(iter->second.getPath()))
			loc = iter->second;

	if (loc.getRoot().empty())
		for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++)
			if (iter->second.isDefault())
				loc = iter->second;

	std::cout << "FOUND LOCATION " << loc.getPath() << std::endl;

	const	std::vector<std::string>& methods = loc.getAllowMethods();
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++) {
		allowedMethods += *it + (it + 1 != methods.end() ? ", " : "");
		if ((allowedMethod = (it->compare(method) ? false : true)))
			break;
	}

	if (!allowedMethod)
		HTTPResponse::generateResponse(405, allowedMethods, "IS_ALIv", sConf);
	else {
		if (loc.getRoot()[0] == '/')
			returnPath = sConf.getRoot() + loc.getRoot() + path.substr(locReq.length());
		else
			returnPath = sConf.getRoot() + "/" + loc.getRoot() + path.substr(locReq.length());
		std::cout << "RETURN PATH IS " << returnPath << std::endl;
		if (stat(returnPath.c_str(), &s) == 0)
			if (s.st_mode & S_IFDIR)
				returnPath += loc.getIndex();
	}
	return returnPath;
}
/*
	---------------------------------------------
			Getters because I love OOP
	---------------------------------------------
*/

const std::map<std::string, std::string>&	HTTPRequest::getHeaders() const
{
	return (_headers);
}

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

size_t	HTTPRequest::getContentLength() const
{
	return (_bodySize);
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
