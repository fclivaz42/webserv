#include "Requests/HTTPRequest.hpp"
#include "Requests/Post.hpp"

/*
	---------------------------------------------
	Constructors, Destructors and all that shizaz
	---------------------------------------------
*/

HTTPRequest::HTTPRequest(	const std::string& method,
							const std::string& path,
							const std::string& version,
							const std::map<std::string, std::string> headers,
							const ServerConf& sConf,
							const Location& loc) : _method(method),
													_path(path),
													_version(version),
													_headers(headers),
													_sConf(sConf),
													_loc(loc)
{}
/*
void	HTTPRequest::fillRequest(std::string& request, bool cont)
{
	std::string	line, key, value;
	size_t		pos, delim, tmp;

	if (cont)
	{
		_method = "POST";
		_body = request;
		request.clear();
		return ;
	}

	if (DEBUG) {
		std::stringstream	stRequest(request.substr(0, request.find_first_not_of(PRINTABLES) - 1));
		std::string			prRequest;
		std::cout << "\n┌────────── NEW REQUEST ──────────\n";
		while (std::getline(stRequest, prRequest))
			if (stRequest.peek() != EOF)
				std::cout << "│ " << prRequest << std::endl;
	}

	delim = request.find("\r\n") + 2;
	std::istringstream requestLine(request.substr(0, delim - 2));
	requestLine >> _method >> _path >> _version;

	_path = urlDecode(_path);

	if (DEBUG) {
		std::cout << "├────────── REQUEST METADATA ──────────\n";
		std::cout << "│ Method: " << "" << _method << "" << std:: endl;
		std::cout << "│ Path: " << _path << std:: endl;
		std::cout << "│ Version: " << _version << "\n└────────── END REQUEST  ──────────\n";
	}
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		HTTPResponse::generateResponse(405, "GET, POST, DELETE", this->isKeepAlive(), *this);

	if (_path.empty() || _path[0] != '/')
		HTTPResponse::generateResponse(400, "", this->isKeepAlive(), *this);

	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		HTTPResponse::generateResponse(505, "", this->isKeepAlive(), *this);

	while (delim < request.length())
	{
		tmp = request.find("\r\n", delim) + 2;
		if (delim >= request.length() || tmp >= request.length())
			break;
		line = request.substr(delim, tmp - delim - 1);
		delim = tmp;
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
				_body = request.substr(delim);
				break;
			}
			else
				continue;
		else if (line.find(_headers["boundary"]) != std::string::npos)
		{
			_body = request.substr(delim);
			break ;
		}
		else
			HTTPResponse::generateResponse(400, "", this->isKeepAlive(), *this);
	}
	if (_headers.find("Referer") != _headers.end()) {
    	size_t pos = _headers.find("Referer")->second.find("?");
    	if (pos != std::string::npos) {
       		std::string name = _headers.find("Referer")->second;
        	std::string _query = name.substr(pos + 1);
			size_t lastPos = name.find_last_of("/", pos);
			if (lastPos != std::string::npos)
            	_fileName = name.substr(lastPos + 1, pos - lastPos - 1);
        	std::cout << "QUERY: " << _query << std::endl;
			std::cout << "FILE: " << _fileName << std::endl;
    	}
	}
	if (_version == "HTTP/1.1" && (_headers.find("Host") == _headers.end()))
		HTTPResponse::generateResponse(400, "", this->isKeepAlive(), *this);

	if (_method == "POST")
	{
		if (_headers.find("Content-Length") == _headers.end())
			HTTPResponse::generateResponse(411, "", this->isKeepAlive(), *this);
		if (_headers.find("Content-Type") == _headers.end())
			HTTPResponse::generateResponse(415, "", this->isKeepAlive(), *this);
	}
		char	*ptr;
		long	testsize = strtol(_headers["Content-Length"].c_str(), &ptr, 10);
		if (testsize < 0 || ptr[0] != 0)
			HTTPResponse::generateResponse(418, "", this->isKeepAlive(), *this);

		_bodySize = strtoul(_headers["Content-Length"].c_str(), NULL, 10);
	request.clear();
}
*/
HTTPRequest::HTTPRequest(HTTPRequest const &copy) :	_method(copy._method),
													_path(copy._path),
													_version(copy._version),
													_headers(copy._headers),
													_createdPath(copy._createdPath),
													_query(copy._query),
													_fileName(copy._fileName),
													_body(copy._body),
													_bodySize(copy._bodySize),
													_sConf(copy._sConf),
													_loc(copy._loc)
{}

HTTPRequest::~HTTPRequest()
{}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &rhs)
{
	if (this != &rhs)
	{
		_body = rhs._body;
		_bodySize = rhs._bodySize;
	}
	return (*this);
}

void	HTTPRequest::createPath()
{
	std::string		returnPath, locReq, allowedMethods;
	struct stat		s;
	const Location&	loc = this->_loc;
	bool			allowedMethod = false;

	locReq = loc.getPath();

	const	std::vector<std::string>& methods = loc.getAllowMethods();
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++) {
		allowedMethods += *it + (it + 1 != methods.end() ? ", " : "");
		if ((allowedMethod = (it->compare(this->_method) ? false : true)))
			break;
	}

	if (!allowedMethod)
		HTTPResponse::generateResponse(405, allowedMethods, "", *this);

	if (!loc.getReturnURL().empty())
		HTTPResponse::generateResponse(302, loc.getReturnURL(), this->isKeepAlive(), *this);

	else {
		if (loc.getRoot()[0] == '/')
			returnPath = this->_sConf.getRoot() + loc.getRoot() + this->_path.substr(locReq.length());
		else
			returnPath = this->_sConf.getRoot() + "/" + loc.getRoot() + this->_path.substr(locReq.length());
		if (DEBUG)
			std::cout << "RETURN PATH IS " << returnPath << std::endl;
		if (stat(returnPath.c_str(), &s) == 0)
			if (s.st_mode & S_IFDIR) {
				if (loc.getIndex().size() > 0)
					returnPath += loc.getIndex();
				else if (loc.hasAutoIndex() || this->_method == "POST")
					;
				else
					HTTPResponse::generateResponse(403, "", this->isKeepAlive(), *this);
			}
	}
	this->_createdPath = returnPath;
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

const std::string&	HTTPRequest::getCreatedPath() const
{
	return (_createdPath);
}

const std::string&	HTTPRequest::getVersion() const
{
	return (_version);
}

const std::string&	HTTPRequest::getBody() const
{
	return (_body);
}

const std::string&	HTTPRequest::getQuery() const
{
	return (_query);
}

const std::string&	HTTPRequest::getFileName() const
{
	return (_fileName);
}


const ServerConf&	HTTPRequest::getSConf() const
{
	return (_sConf);
}

/*
	---------------------------------------------
			Setters because I love OOP
	---------------------------------------------
*/

void	HTTPRequest::setQuery(const std::string& query){
	this->_query = query;
}

void	HTTPRequest::setFileName(const std::string& name){
	this->_fileName = name;
}

void	HTTPRequest::setBodySize(size_t size)
{
	this->_bodySize = size;
}

void	HTTPRequest::setCreatedPath(const std::string& cPath)
{
	this->_createdPath = cPath;
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
