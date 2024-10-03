#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Network/ConnectManager.hpp"
#include "Requests/HttpRequest.hpp"
#include <sys/stat.h>
#include <unistd.h>

std::string			processGetRequest(const HttpRequest& request, const ServerConf& serverConf);
std::string			getMimeType(const std::string& path);
bool				hasExtension(const std::string& path, const std::string& extension);
std::string			updatePath(const std::string &path);
const std::string	createPath(const std::string& path, const ServerConf& serverConf, const std::string& method);
bool				fileExists(const std::string& localPath);

#endif
