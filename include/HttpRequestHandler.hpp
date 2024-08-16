// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpRequestHandle.hpp                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 14:03:53 by lmedrano          #+#    #+#             //
//   Updated: 2024/08/15 14:05:47 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef HTTPREQUESTHANDLER_HPP

# define HTTPREQUESTHANDLER_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"

class HttpRequestHandler
{
	public:
		static std::string handleRequest(const std::string& request);
};

#endif
