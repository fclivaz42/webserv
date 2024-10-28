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
		std::string							_createdPath;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		size_t								_bodySize;
		const ServerConf&					_sConf;
		Location							_loc;
		

	public:

		HTTPRequest(std::string& request, const ServerConf& sConf, bool cont);
		HTTPRequest(HTTPRequest const &copy);
		HTTPRequest &operator=(HTTPRequest const &rhs);
		~HTTPRequest();

		std::string	createPath(const std::string& path,
										const std::string& method,
										bool attrib);
		void 				checkRedir(const std::string& path,
										const std::string& method);

		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string							getContent(std::string wich) const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getVersion() const;
		const std::string&							getBody();
		const std::string&							getQuery() const;
		const std::string&							getFileName() const;
		const std::string&							getCreatedPath() const;
		const Location&								getLoc() const;
		const ServerConf&							getSConf() const;
		void										setQuery(std::string query);
		void										setCreatedPath(std::string path);
		void										setFileName(std::string name);
		void										setBody(std::string body);
		void										setHeaders(std::string wich, std::string content);
		const std::string							isKeepAlive() const;
		size_t										getContentLength() const;

};

std::string	trim(const std::string& str);

#define PRINTABLES "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=!@#$%^&*()_+[]\\;',.{}|:\"<>?/ \f\r\n\t\v"

#endif
