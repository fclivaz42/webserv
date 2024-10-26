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

void	fileCheck(const std::string& localPath, const HTTPRequest& request)
{
	struct stat	fileInfo;

	if (stat(localPath.c_str(), &fileInfo) != 0)
	{
		std::cerr << RED << "GET: ERROR: File not found: " << localPath << RESET << std::endl;
		HTTPResponse::generateResponse(404, "", request.isKeepAlive(), request);
	}
	else if (access(localPath.c_str(), R_OK) != 0)
	{
		std::cerr << RED << "GET: ERROR: Access denied to file: " << localPath << RESET << std::endl;
		HTTPResponse::generateResponse(403, "", request.isKeepAlive(), request);
	}
	else if (!(S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode)))
	{
		std::cerr << RED << "GET: ERROR: Not a regular file." << RESET << std::endl;
		HTTPResponse::generateResponse(500, "", request.isKeepAlive(), request);
	}
}

bool	checkRedir(const std::string& path, const ServerConf& sConf)
{
	std::map<std::string, Location> locationMap = sConf.getLocation();

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++)
	{
		const	Location& loc = iter->second;
		std::string ret = loc.getReturnURL();
		if (!ret.empty() && ret == path)
			return (true);
	}
	return (false);
}

std::string	processGetRequest(const HTTPRequest& request)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::string		localPath;

	if (static_cast<size_t>(strtol(headers["Content-Length"].c_str(), NULL, 10)) > request.getSConf().getMaxBodySize())
		HTTPResponse::generateResponse(413, "", request.isKeepAlive(), request);
	
	std::string path = request.getPath();

	if (checkRedir(path, request.getSConf()))
		return (HTTPResponse::generateResponse(302, path, request.isKeepAlive(), request));

	if (DEBUG)
		std::cout << GREEN << "GET: PATH IS: " << path << RESET << std::endl;

	localPath = request.createPath(path, "GET", false);

	if (DEBUG)
		std::cout << GREEN << "GET: Created Local Path: " << localPath << RESET << std::endl;

	fileCheck(localPath, request);

	return (HTTPResponse::generateResponse(200, localPath, request.isKeepAlive(), request));
}
