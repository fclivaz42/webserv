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

const std::string	processPostRequest(const HTTPRequest& request);
const std::string	urlDecode(const std::string& str);

#endif
