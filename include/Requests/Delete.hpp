#ifndef DELETE_HPP
# define DELETE_HPP

#include <string>
#include "Network/ConnectManager.hpp"
#include "Requests/HTTPRequest.hpp"

std::string	processDeleteRequest(const HTTPRequest& request);

#endif
