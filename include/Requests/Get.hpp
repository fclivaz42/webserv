#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"
#include <sys/stat.h>
#include <unistd.h>
# define MAX_BODY_SIZE 10485760

std::string	processGetRequest(const HttpRequest& request, const ServerConf& serverConf);
std::string	getMimeType(const std::string& path);
bool		hasExtension(const std::string& path, const std::string& extension);
std::string	updatePath(const std::string &path);
const std::string createPath(const std::string& path, const ServerConf& serverConf);

#endif
