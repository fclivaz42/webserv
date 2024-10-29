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
		const std::string							_method;
		const std::string							_path;
		const std::string							_version;
		const std::map<std::string, std::string>	_headers;
		std::string									_createdPath;
		std::string									_query;
		std::string									_body;
		size_t										_bodySize;
		const ServerConf&							_sConf;
		const Location&								_loc;

	public:
		HTTPRequest(std::map<std::string, std::string>& attribs,
					const std::map<std::string, std::string>& headers,
					const ServerConf& sConf,
					const Location& loc);
		HTTPRequest(const std::string&line, const ServerConf& sConf, const Location& loc);
		HTTPRequest(HTTPRequest const &copy);
		HTTPRequest &operator=(HTTPRequest const &rhs);
		~HTTPRequest();

		void	createPath();

		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getCreatedPath() const;
		const std::string&							getVersion() const;
		const std::string&							getBody() const;
		const std::string&							getQuery() const;
		const ServerConf&							getSConf() const;
		const Location&								getLoc() const;
		void										setQuery(const std::string& query);
		void										setBodySize(size_t size);
		void										setCreatedPath(const std::string& cPath);
		const std::string							isKeepAlive() const;
		size_t										getContentLength() const;

};

std::string	trim(const std::string& str);

#define PRINTABLES "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=!@#$%^&*()_+[]\\;',.{}|:\"<>?/ \f\r\n\t\v"

#endif
