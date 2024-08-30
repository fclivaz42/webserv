// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Get.hpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 15:11:49 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/30 15:16:17 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GET_HPP
# define GET_HPP

#include <string>
#include "Sockets/SocketManager.hpp"

std::string	processGetRequest(std::string path, std::string connectionHandler);
std::string	getMimeType(const std::string& path);
bool		hasExtension(const std::string& path, const std::string& extension);

#endif
