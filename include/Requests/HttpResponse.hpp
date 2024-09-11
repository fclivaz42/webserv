#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Parsing/ServerConf.hpp"
#include "webserv.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class HttpResponse
{
	public:
		HttpResponse(const ServerConf& serverConf);
		std::string	generateResponse(int statusCode, std::string& path, std::string &alive);
		std::string	getErrorPage(const std::string& errorCode) const;

	private:
		std::string getContentType(const std::string& path);
		std::string	readFileContent(const std::string& path) const;
		const ServerConf&	serverConf;
};

#endif
