#ifndef POST_HPP
# define POST_HPP

#include <string>
#include <map>
#include "Parsing/ServerConf.hpp"
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"

const std::string	processPostRequest(const HttpRequest& request, const ServerConf& serverConf);

#endif
