#include "Requests/Get.hpp"

std::string	updatePath(const std::string &path)
{
	if (path == "/")
		return ("./public/index.html");
	else if (path.find("public") == std::string::npos)
		return ("./public" + path);
	else
		return ("404 not found");
	return (path);
}

bool	fileExists(const std::string& localPath)
{
	bool isOpen;
	std::ifstream file(localPath.c_str());

	if (file.good())
		isOpen = true;
	else
		isOpen = false;
	file.close();
	return isOpen;
}

bool	hasAccess(const std::string& localPath)
{
	struct stat	fileInfo;

	if (stat(localPath.c_str(), &fileInfo) != 0)
	{
		std::cerr << RED << "GET: ERROR: file does not exist." << RESET << std::endl;
		return (false);
	}
	else if (access(localPath.c_str(), R_OK) != 0)
	{
		std::cerr << RED << "GET: ERROR: No access to file." << RESET << std::endl;
		return (false);
	}
	else if (!S_ISREG(fileInfo.st_mode))
	{
		std::cerr << RED << "GET: ERROR: Not a regular file." << RESET << std::endl;
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

std::string	processGetRequest(const HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::string		localPath;

	if (static_cast<size_t>(strtol(headers["Content-Length"].c_str(), NULL, 10)) > serverConf.getMaxBodySize())
		HTTPResponse::generateResponse(413, "", request.isKeepAlive(), serverConf);
	//TODO send http error response instead
	
	std::string path = request.getPath();

	if (checkRedir(path, serverConf))
		return (HTTPResponse::generateResponse(302, path, request.isKeepAlive(), serverConf));

	if (DEBUG)
		std::cout << GREEN << "GET: PATH IS: " << path << RESET << std::endl;

	localPath = HTTPRequest::createPath(path, serverConf, "GET", "SWAg");

	if (DEBUG)
		std::cout << GREEN << "GET: Created Local Path: " << localPath << RESET << std::endl;

	if (!fileExists(localPath))
	{
		std::cerr << RED << "GET: ERROR: File not found: " << localPath << RESET << std::endl;
		HTTPResponse::generateResponse(404, "", request.isKeepAlive(), serverConf);
	}
	if (!hasAccess(localPath))
	{
		std::cerr << RED << "GET: ERROR: Access denied to file: " << localPath << RESET << std::endl;
		HTTPResponse::generateResponse(403, "", request.isKeepAlive(), serverConf);
	}
	return (HTTPResponse::generateResponse(200, localPath , request.isKeepAlive(), serverConf));
}
