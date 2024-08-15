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

class HttpRequestHandler
{
	public:
		static std::string handleRequest(const std::string& request);
};

#endif
