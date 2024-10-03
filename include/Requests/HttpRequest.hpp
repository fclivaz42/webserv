#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "webserv.hpp"
#include <string>
#include <map>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>

class	HttpRequest
{
	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::stringstream					_body;

		void			specialPostParsing();
		std::string 	getBoundary(const std::string& contentType);
		void 	 		parseMultiPartBody(const std::string& body, const std::string& boundary);
		HttpRequest();

	public:
		HttpRequest(std::stringstream& request, bool cont);
		HttpRequest(HttpRequest const &copy);
		HttpRequest &operator=(HttpRequest const &rhs);
		~HttpRequest();

		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getVersion() const;
		std::stringstream&							getBody();
		bool										isKeepAlive() const;
};

std::string	trim(const std::string& str);

#endif
