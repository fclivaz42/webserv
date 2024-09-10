#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "webserv.hpp"
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>

class	HttpRequest
{
	private:
		std::string method;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;

		void	parseRequest(const std::string& request);
	public:
		HttpRequest();
		HttpRequest(const std::string& request);
		/* COPY CONSTRUCTOR */
		HttpRequest(HttpRequest const &copy);
		/* DESTRUCTOR */
		~HttpRequest();
		/* SURCHARGED OPERATORS */
		HttpRequest &operator=(HttpRequest const &rhs);
		/* GETTERS */
		std::string				getMethod() const;
		std::string				getPath() const;
		std::string				getVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string				getBody() const;
		bool					isKeepAlive() const;
};

#endif
