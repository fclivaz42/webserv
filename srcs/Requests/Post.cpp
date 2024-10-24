// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Post.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/10/24 13:58:01 by lmedrano          #+#    #+#             //
//   Updated: 2024/10/24 15:24:03 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Post.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

static std::string urlDecode(const std::string& str)
{
	std::string	result;
	size_t		length = str.length();

	for (size_t i = 0; i < length; ++i)
	{
		if (str[i] == '%')
		{
			if (i + 2 < length)
			{
				int value;
				std::istringstream is(str.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
					result += static_cast<char>(value);
					i += 2;
				}
			}
		}
		else if (str[i] == '+')
			result += ' ';
		else
			result += str[i];
	}
	return result;
}

static const std::string	uploadRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	const std::string&					shift = request.getBody().str();
	std::string							fileName, line, path;

	if (DEBUG)
		std::cout << GREEN << "POST: File being uploaded." << std::endl;
	fileName = shift.substr(shift.find("filename=\"") + 10);
	fileName = fileName.substr(0, fileName.find_first_of("\""));
	path = HTTPRequest::createPath(request.getPath() + fileName, serverConf, "POST", true);
	if (DEBUG)
		std::cout << "POST: Created path: " << path << RESET << std::endl;

	std::ofstream outFile(path.c_str(), std::ios::out | std::ios::binary);
	if (outFile.is_open()) {
		size_t pos = shift.find("\r\n\r\n") + 4;
		size_t pos2 = shift.find(headers["boundary"]) - pos - 4;
		outFile.write(shift.c_str() + pos, pos2);
		outFile.close();
	}
	else
		HTTPResponse::generateResponse(500, "", request.isKeepAlive(), serverConf);
	return HTTPResponse::generateResponse(201, path, request.isKeepAlive(), serverConf);
}

static const std::string	formRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> formData;
	std::stringstream& bodyStream(request.getBody());
	std::string keyValue, username, email, message;

	if (DEBUG)
		std::cout << GREEN << "POST: Form received.\n";
	while (std::getline(bodyStream, keyValue, '&'))
	{
		size_t pos = keyValue.find('=');
		if (pos != std::string::npos)
		{
			std::string key = urlDecode(keyValue.substr(0, pos));
			std::string value = urlDecode(keyValue.substr(pos + 1));
			formData[key] = value;
		}
	}

	username = formData["name"];
	email = formData["email"];
	message = formData["message"];

	std::string			path(HTTPRequest::createPath(request.getPath(), serverConf, "POST", false)), line, response;
	std::stringstream	genRes(HTTPResponse::generateResponse(200, path, request.isKeepAlive(), serverConf));
	std::size_t			pos;

	while (std::getline(genRes, line))
	{
		pos = line.find("+username+");
		if (pos != std::string::npos)
			line.replace(pos, 10, username);
		pos = line.find("+email+");
		if (pos != std::string::npos)
			line.replace(pos, 7, email);
		pos = line.find("+message+");
		if (pos != std::string::npos)
			line.replace(pos, 9, message);

		response += line + '\n';
	}
	return (response);
}

static std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

static std::map<std::string, std::string> createCGIEnv(HTTPRequest& request)
{
    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"] = "GET";
    env["CONTENT_TYPE"] = request.getHeaders().at("Content-Type");
    env["CONTENT_LENGTH"] = intToString(request.getBody().tellp());
    env["SCRIPT_NAME"] = request.getPath();
    env["REQUEST_URI"] = request.getPath();
	env["QUERY_STRING"] = "";
	env["PATH_INFO"] = request.getPath();
	env["PATH TRANSLATED"] = request.getPath();

    return (env);
}

static bool		isCGIRequest(const std::string& path)
{
		std::string cgiPath = "/calculator.html";
	return (path.find(cgiPath) == 0);
}

const std::string	processPostRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	std::cout << "BLABLA " << std::endl;
	std::cout << "REQUEST: " << request.getPath() << std::endl;
	if (isCGIRequest(request.getPath()))
	{
		std::string cgiPath = "/cgi-bin/script.py";
        	std::map<std::string, std::string> env = createCGIEnv(request);
		CGIExec cgiExec(cgiPath, env);;
		return (cgiExec.execute(request.getBody().str()));
	}
	if (headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
		return (formRequest(request, serverConf));
	else if (headers["Content-Type"].find("multipart") != std::string::npos)
		return (uploadRequest(request, serverConf));
	return HTTPResponse::generateResponse(415, "", request.isKeepAlive(), serverConf);
}
