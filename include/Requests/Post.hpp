#ifndef POST_HPP
# define POST_HPP

#include <map>
#include <string>
#include <sys/stat.h>
#include "CGI/CGIExec.hpp"
#include "Parsing/Location.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

const std::string	processPostRequest(HTTPRequest& request, const ServerConf& serverConf);

#endif
