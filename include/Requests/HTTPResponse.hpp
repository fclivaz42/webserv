#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Parsing/ServerConf.hpp"
#include "webserv.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class HTTPResponse
{
	public:
		HTTPResponse(const ServerConf& serverConf);
		std::string	generateResponse(const std::string& statusCode, std::string& path, const std::string &alive);
		std::string	loadErrorPage(const std::string& errorCode) const;

	private:
		std::string getContentType(const std::string& path);
		std::string	readFileContent(const std::string& path) const;
		const ServerConf&	serverConf;
};

#endif
