// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.hpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 15:13:27 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 15:31:29 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef POST_HPP
# define POST_HPP

#include <string>
#include <map>
#include "Sockets/HttpRequestHandler.hpp"
#include "Sockets/SocketManager.hpp"

std::string	processPostRequest(std::string body, std::string connectionHandler);

#endif
