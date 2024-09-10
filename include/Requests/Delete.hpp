#ifndef DELETE_HPP
# define DELETE_HPP

#include <string>
#include <map>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processDeleteRequest(const HttpRequest& request);

#endif
