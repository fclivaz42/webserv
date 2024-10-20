#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Parsing/ServerConf.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

bool				fileExists(const std::string& localPath);
std::string			updatePath(const std::string &path);
std::string			processGetRequest(const HTTPRequest& request, const ServerConf& serverConf);

#endif
