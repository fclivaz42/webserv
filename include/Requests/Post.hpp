#ifndef POST_HPP
# define POST_HPP

#include <string>
#include <map>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processPostRequest(const HttpRequest& request);

#endif
