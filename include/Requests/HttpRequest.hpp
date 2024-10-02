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
		std::string method;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;

		void			parseRequest(std::stringstream& request);
		void			specialPostParsing();
		std::string 	getBoundary(const std::string& contentType);
		void 	 		parseMultiPartBody(const std::string& body, const std::string& boundary);

	public:
		HttpRequest();
		HttpRequest(std::stringstream& request);
		/* COPY CONSTRUCTOR */
		HttpRequest(HttpRequest const &copy);
		/* DESTRUCTOR */
		~HttpRequest();
		/* SURCHARGED OPERATORS */
		HttpRequest &operator=(HttpRequest const &rhs);
		/* GETTERS */
		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getVersion() const;
		const std::string&							getBody() const;
		bool										isKeepAlive() const;
};

std::string	trim(const std::string& str);

#endif
