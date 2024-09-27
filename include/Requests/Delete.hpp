#ifndef DELETE_HPP
# define DELETE_HPP

#include <string>
#include <map>
#include "Network/ConnectManager.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processDeleteRequest(const HttpRequest& request);

#endif
