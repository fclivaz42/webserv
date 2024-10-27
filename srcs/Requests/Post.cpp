// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
/*   Post.cpp                                           :+:      :+:    :+:   */
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/10/24 13:58:01 by lmedrano          #+#    #+#             //
/*   Updated: 2024/10/27 23:23:04 by fclivaz          ###   LAUSANNE.ch       */
//                                                                            //
// ************************************************************************** //

#include "Requests/Post.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

const std::string urlDecode(const std::string& str)
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

static const std::string	uploadRequest(HTTPRequest& request)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	const std::string&					shift = request.getBody();
	std::string							fileName, line, path;

	if (DEBUG)
		std::cout << GREEN << "POST: File being uploaded." << std::endl;
	if (shift.find("filename=\"") == std::string::npos) {
		std::cout << "FILENAME WAS NOT FOUND!! HERE'S WHAT WE GOT:\n";
		std::string tej;
		std::stringstream bodyStream(request.getBody());
		while (tej != "\r") {
			std::getline(bodyStream, tej);
			std::cout << tej << "\n";
		}
	}
	fileName = shift.substr(shift.find("filename=\"") + 10);
	fileName = fileName.substr(0, fileName.find_first_of("\""));
	path = request.getPath();
	path = request.createPath(path + (*(path.end() - 1) == '/' ? "" : "/") + fileName, "POST", true);
	if (DEBUG)
		std::cout << "POST: Created path: " << path << RESET << std::endl;

	std::ofstream outFile(path.c_str(), std::ios::out | std::ios::binary);
	if (outFile.is_open()) {
		size_t pos = shift.find("\r\n\r\n") + 4;
		if (shift.find(headers["boundary"]) != std::string::npos) {
			size_t pos2 = shift.find(headers["boundary"]) - pos - 4;
			outFile.write(shift.c_str() + pos, pos2);
			outFile.close();
		}
		else {
			outFile.write(shift.c_str() + pos, shift.length() - pos);
			outFile.close();
		}
	}
	else
		HTTPResponse::generateResponse(500, "", request.isKeepAlive(), request);
	return HTTPResponse::generateResponse(201, path, request.isKeepAlive(), request);
}

static const std::string	formRequest(HTTPRequest& request)
{
	std::map<std::string, std::string> formData;
	std::stringstream bodyStream(request.getBody());
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

	std::string			path(request.createPath(request.getPath(), "POST", false)), line, response;
	std::stringstream	genRes(HTTPResponse::generateResponse(200, path, request.isKeepAlive(), request));
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
    env["CONTENT_LENGTH"] = intToString(request.getBody().size());
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

const std::string	processPostRequest(HTTPRequest& request)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	std::cout << "POST REQUEST: " << request.getPath() << std::endl;
//	(void)request.createPath(request.getPath(), "POST", false);
	if (isCGIRequest(request.getPath()))
	{
		std::string cgiPath = "/cgi-bin/script.py";
        	std::map<std::string, std::string> env = createCGIEnv(request);
		CGIExec cgiExec(cgiPath, env);;
		return (cgiExec.execute(request.getBody()));
	}
	if (headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
		return (formRequest(request));
	else if (headers["Content-Type"].find("multipart") != std::string::npos)
		return (uploadRequest(request));
	return HTTPResponse::generateResponse(415, "", request.isKeepAlive(), request);
}
