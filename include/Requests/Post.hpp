// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.hpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 15:13:27 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:16:26 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef POST_HPP
# define POST_HPP

#include <string>
#include <map>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"

std::string	processPostRequest(const HttpRequest& request);

#endif
