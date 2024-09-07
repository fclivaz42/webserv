// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   HttpResponse.hpp                                   :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/07 18:19:29 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/07 18:38:40 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef HTTPRESPONSE_HPP

# define HTTPRESPONSE_HPP

#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define ORANGE "\x1b[38;5;208m"
#define RED "\x1b[31m"
#define PURPLE "\x1b[35m"

#include "Parsing/ServerConf.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class HttpResponse
{
	public:
		HttpResponse(const ServerConf& serverConf);
		std::string		generateResponse(const std::string& statusCode, const std::string& path) const;
		std::string		getErrorPage(const std::string& errorCode) const;
	private:
		std::string		getContentType(const std::string& path) const;
		std::string		readFileContent(const std::string& path) const;
		const ServerConf&	serverConf;
};

#endif
