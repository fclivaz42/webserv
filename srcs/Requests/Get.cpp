// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   Get.cpp                                            :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: lmedrano <lmedrano@student.42lausanne.ch>  +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2024/08/30 14:33:20 by lmedrano          #+#    #+#             //
//   Updated: 2024/09/04 11:31:53 by lmedrano         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "Requests/Get.hpp"
#include "Requests/HttpRequest.hpp"
#include "Parsing/Location.hpp"

bool		hasExtension(const std::string& path, const std::string& extension)
{
	std::string::size_type pos = path.rfind(extension);
	return ((pos != std::string::npos) && (pos == path.length() - extension.length()));
}

std::string	getMimeType(const std::string& path)
{
	if (hasExtension(path, ".css"))
		return ("text/css");
	if (hasExtension(path, ".html"))
		return ("text/html");
	if (hasExtension(path, ".js"))
		return ("application/javascript");
	if (hasExtension(path, ".png"))
		return ("image/png");
	if (hasExtension(path, ".jpg") || hasExtension(path, ".jpeg"))
		return ("image/jpeg");
	if (hasExtension(path, ".gif"))
		return ("image/gif");
	return ("application/octet-stream");
}

std::string	updatePath(const std::string &path)
{
	if (path == "/")
		return ("/public/index.html");
	else if (path.find("public") == std::string::npos)
		return ("/public" + path);
	else
		return ("404 not found");
	return (path);
}

bool		fileExists(std::string localPath)
{
	std::ifstream file(localPath.c_str());

	file.open(localPath.c_str());
	return (file.good());
}

bool		hasAccess(std::string localPath)
{
	struct stat	fileInfo;;

	if (stat(localPath.c_str(), &fileInfo) != 0)
	{
		std::cerr << RED << "ERROR: file does not exist." << RESET << std::endl;
		return (false);
	}
	else if (access(localPath.c_str(), R_OK) != 0)
	{
		std::cerr << RED << "ERROR: No access to file." << RESET << std::endl;
		return (false);
	}
	else if (!S_ISREG(fileInfo.st_mode))
	{
		std::cerr << RED << "ERROR: Not a regualr file." << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool	checkRedir(const std::string& path, const ServerConf& serverConf)
{
	std::map<std::string, Location> locationMap = serverConf.getLocation();

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++)
	{
		const	Location& loc = iter->second;
		std::string ret = loc.getReturnURL();
		if (!ret.empty() && ret == path)
		{
			return (true);
		}
	}
	return (false);
}

std::string	createLocalPathFromLocation(const std::string&path, const ServerConf& serverConf)
{
	const std::map<std::string, Location>& locations = serverConf.getLocation();
	
	for (std::map<std::string, Location>::const_iterator iter = locations.begin(); iter != locations.end(); iter++)
	{
		const Location& loc = iter->second;
		if (path.find(iter->first) == 0)
		{
			std::string localPath = loc.getRoot() + path.substr(iter->first.length());
			return (localPath);
		}
	}
	return ("");
}

std::string 	createLocalPathFromRoot(const std::string& path)
{
	std::string	serverRoot = "/public";

	if (path == "/")
		return (serverRoot + "/index.html");
	return (serverRoot + path);
}


bool 		isLocationPath(const std::string& path, const ServerConf& serverConf)
{
	const std::map<std::string, Location>& locations = serverConf.getLocation();
	return (locations.find(path) != locations.end());
}

std::string	processGetRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	if (request.getBody().size() > MAX_BODY_SIZE)
		return ("HTTP/1.1 ERROR 413 Payload Too Large\r\nConnection: close\r\n\r\n");
	//TODO send http error response instead
	
	std::string path = request.getPath();

	if (checkRedir(path, serverConf))
	{
		return ("HTTP/1.1 ERROR 302 Found\r\nLocation: " + path + "\r\nConnection: close\r\n\r\n");
	}

	std::cout << GREEN << path << RESET << std::endl;

	std::string localPath;
	if (isLocationPath(path, serverConf))
	{
		localPath = createLocalPathFromLocation(path, serverConf);
		std::cout << GREEN << localPath << RESET << std::endl;
	}
	else
	{
		localPath = createLocalPathFromRoot(path);
		std::cout << PURPLE << localPath << RESET << std::endl;
	}
	//if (!fileExists(path))
	//{
	//	std::cerr << RED << "ERROR: File not found: " << path << RESET << std::endl;
	//	return ("HTTP/1.1 ERROR 404 Page not found\r\nConnection: close\r\n\r\n");
	//}
	//if (!hasAccess(path))
	//{
	//	std::cerr << RED << "ERROR: Access denied to file: " << localPath << RESET << std::endl;
	//	return ("HTTP/1.1 ERROR 403 Forbidden\r\nConnection: close\r\n\r\n");
	//}

	std::string fileContent = SocketManager::readFile("." + localPath);
	std::cout << PURPLE << "fileContent is: " << fileContent << RESET << std::endl; 

	std::string contentType = getMimeType(localPath);
	std::cout << PURPLE << "contentType is: " << contentType << RESET << std::endl; 

	std::string alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + fileContent);
}
