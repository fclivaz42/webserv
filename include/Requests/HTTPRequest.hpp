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

class	HTTPRequest
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
		HTTPRequest();

	public:
		HTTPRequest(std::stringstream& request, bool cont);
		HTTPRequest(HTTPRequest const &copy);
		HTTPRequest &operator=(HTTPRequest const &rhs);
		~HTTPRequest();

		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&							getMethod() const;
		const std::string&							getPath() const;
		const std::string&							getVersion() const;
		std::stringstream&							getBody();
		const std::string							isKeepAlive() const;

		/* EXCEPTIONS */
		class RequestNotAllowed : public std::exception{
			private:
				std::string	_str;
			public:
				RequestNotAllowed(const std::string& str) {
					this->_str = "HTTP/1.1 405 Method Not Allowed\r\nAllow: " + str + "\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
				}
				~RequestNotAllowed() throw () {}
				virtual char const	*what(void) const throw() {
					return _str.c_str();
				}
		};
		class UnsupportedHTTPVersion : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}
		};
		class InvalidHeaders : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}
		};
		class MissingHost : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}
		};
};

std::string	trim(const std::string& str);

#endif
