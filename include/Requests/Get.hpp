#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Parsing/ServerConf.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

std::string			processGetRequest(HTTPRequest& request);

#endif
