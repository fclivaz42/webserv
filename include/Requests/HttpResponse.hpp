// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/09/07 18:19:29 by lmedrano          #+#    #+#             //
/*   Updated: 2024/09/10 15:17:45 by fclivaz          ###   LAUSANNE.ch       */
//                                                                            //
// ************************************************************************** //

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Parsing/ServerConf.hpp"
#include "webserv.hpp"
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
