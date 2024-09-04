// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Delete.hpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/04 10:45:37 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:17:13 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef DELETE_HPP
# define DELETE_HPP

#include <string>
#include <map>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processDeleteRequest(const HttpRequest& request);

#endif
