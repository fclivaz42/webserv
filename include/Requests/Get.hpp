// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Get.hpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 15:11:49 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 10:48:03 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Sockets/SocketManager.hpp"
#include "Requests/HttpRequest.hpp"
#include <sys/stat.h>
#include <unistd.h>
# define MAX_BODY_SIZE 10485760

std::string	processGetRequest(const HttpRequest& request, const ServerConf& serverConf);
std::string	getMimeType(const std::string& path);
bool		hasExtension(const std::string& path, const std::string& extension);
std::string	updatePath(const std::string &path);

#endif
