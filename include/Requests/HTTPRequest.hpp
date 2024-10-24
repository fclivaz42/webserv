#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <unistd.h>
#include <sys/stat.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Parsing/Location.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/HTTPResponse.hpp"
#include "webserv.hpp"

class	HTTPRequest
{
	private:
		std::string							_method;
		std::string							_path;
		std::string							_query;
		std::string							_fileName;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::stringstream					_body;
		size_t								_bodySize;

		HTTPRequest();

	public:
		HTTPRequest(std::stringstream& request, const ServerConf& sConf, bool cont);
		HTTPRequest(HTTPRequest const &copy);
		HTTPRequest &operator=(HTTPRequest const &rhs);
		~HTTPRequest();

		static const std::string	createPath(const std::string& path, const ServerConf& serverConf, const std::string& method, bool attrib);

		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getVersion() const;
		std::stringstream&							getBody();
		const std::string&							getQuery() const;
		const std::string&							getFileName() const;
		void										setQuery(std::string query);
		void										setFileName(std::string name);
		const std::string							isKeepAlive() const;
		size_t										getContentLength() const;

};

std::string	trim(const std::string& str);

#endif
