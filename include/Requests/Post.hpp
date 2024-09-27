#ifndef POST_HPP
# define POST_HPP

#include <map>
#include <string>
#include <sys/stat.h>
#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"
#include "Network/ConnectManager.hpp"
#include "Requests/HttpRequest.hpp"
#include "Requests/HttpResponse.hpp"

const std::string	processPostRequest(const HttpRequest& request, const ServerConf& serverConf);
const std::string	createPath(const std::string& path, const ServerConf& serverConf, const std::string& method);

#endif
