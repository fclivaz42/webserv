#include "Requests/HTTPRequest.hpp"
#include "Requests/Post.hpp"

/*
	---------------------------------------------
	Constructors, Destructors and all that shizaz
	---------------------------------------------
*/

HTTPRequest::HTTPRequest(	std::map<std::string, std::string>& attribs,
							const std::map<std::string, std::string>& headers,
							const ServerConf& sConf,
							const Location& loc) : _method(attribs["method"]),
													_path(attribs["path"]),
													_version(attribs["version"]),
													_headers(headers),
													_query(attribs["query"]),
													_body(attribs["body"]),
													_sConf(sConf),
													_loc(loc)
{}

HTTPRequest::HTTPRequest(HTTPRequest const &copy) :	_method(copy._method),
													_path(copy._path),
													_version(copy._version),
													_headers(copy._headers),
													_createdPath(copy._createdPath),
													_query(copy._query),
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
	size_t			len;

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

	len = locReq.length();

	if (len > this->_path.length())
		len = this->_path.length();

	if (loc.getRoot()[0] == '/')
		returnPath = this->_sConf.getRoot() + loc.getRoot() + this->_path.substr(len);
	else
		returnPath = this->_sConf.getRoot() + "/" + loc.getRoot() + this->_path.substr(len);
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

const ServerConf&	HTTPRequest::getSConf() const
{
	return (_sConf);
}

const Location&	HTTPRequest::getLoc() const
{
	return (_loc);
}

/*
	---------------------------------------------
			Setters because I love OOP
	---------------------------------------------
*/

void	HTTPRequest::setQuery(const std::string& query){
	this->_query = query;
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
