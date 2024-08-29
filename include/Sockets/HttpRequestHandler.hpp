// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <your@email.com>                  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/15 14:03:53 by lmedrano          #+#    #+#             //
/*   Updated: 2024/08/29 20:58:25 by fclivaz          ###   LAUSANNE.ch       */
//                                                                            //
// ************************************************************************** //

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

class HttpRequestHandler
{
	public:
		static std::string handleRequest(const std::string& request);
};

#endif
