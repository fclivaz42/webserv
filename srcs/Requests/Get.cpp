#include "Requests/Get.hpp"
#include "Requests/HttpRequest.hpp"
#include "Requests/HttpResponse.hpp"
#include "Parsing/Location.hpp"

bool		hasExtension(const std::string& path, const std::string& extension)
{
	std::string::size_type pos = path.rfind(extension);
	return ((pos != std::string::npos) && (pos == path.length() - extension.length()));
}

std::string	getMimeType(const std::string& path)
{
	std::cout << "PATH-TYPE: " << path << std::endl;
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
		return ("./public/index.html");
	else if (path.find("public") == std::string::npos)
		return ("./public" + path);
	else
		return ("404 not found");
	return (path);
}

bool		fileExists(const std::string& localPath)
{
	bool isOpen;
	std::ifstream file(localPath.c_str());

	std::cout << localPath.c_str() << "\n";
	if (file.good()) {
		isOpen = true;
		std::cout << "FILE IS OPEN\n";
	}
	else {
		isOpen = false;
		std::cout << "FILE IS NOT OPEn\n";
	}
	file.close();
	return isOpen;
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

std::string 	createLocalPathFromRoot(const std::string& path, const ServerConf& serverConf)
{
	const std::map<std::string, Location>& locations = serverConf.getLocation();
	std::string	serverRoot = "/public";

	if (path == "/" || path.empty())
		return (serverRoot + "/index.html");
	return (serverRoot + path);
}


bool 	isLocationPath(const std::string& path, const ServerConf& serverConf)
{
	const std::map<std::string, Location>& locations = serverConf.getLocation();
	return (locations.find(path) != locations.end());
}

std::string	processGetRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	HttpResponse	ret(serverConf);
	std::string alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";
	std::string vide = "";
	if (request.getBody().size() > serverConf.getMaxBodySize())
		return (ret.generateResponse(413, vide, alive));
	//TODO send http error response instead
	
	std::string path = request.getPath();

	if (checkRedir(path, serverConf))
		return (ret.generateResponse(302, path, alive));

	std::cout << GREEN << "PATH IS: " << path << RESET << std::endl;

	std::string localPath;
	if (isLocationPath(path, serverConf))
	{
		localPath = createLocalPathFromLocation(path, serverConf);
		std::cout << GREEN << "Created Local Path: " << localPath << RESET << std::endl;
	}
	else
	{
		localPath = createLocalPathFromRoot(path, serverConf);
		std::cout << PURPLE << "Created Local Path from ROOT: " << localPath << RESET << std::endl;
	}
	//if (!fileExists(localPath))
	//{
	//	std::cerr << RED << "ERROR: File not found: " << localPath << RESET << std::endl;
	//	return (ret.generateResponse(404, "", alive));
	//}
	//if (!hasAccess(localPath))
	//{
	//	std::cerr << RED << "ERROR: Access denied to file: " << localPath << RESET << std::endl;
	//	return (ret.generateResponse(403, "", alive));
	//}
	return (ret.generateResponse(200, localPath , alive));
	//std::string fileContent = SocketManager::readFile("." + localPath);
	//std::cout << PURPLE << "fileContent is: " << fileContent << RESET << std::endl; 

	//std::string contentType = getMimeType(localPath);
	//std::cout << PURPLE << "contentType is: " << contentType << RESET << std::endl; 

	//std::cout << "CONTENTTYPE : " << contentType << std::endl;

	
	//return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + fileContent);
}
