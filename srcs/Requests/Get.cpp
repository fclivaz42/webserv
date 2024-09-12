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

	if (file.good())
		isOpen = true;
	else
		isOpen = false;
	file.close();
	return isOpen;
}

bool		hasAccess(std::string localPath)
{
	struct stat	fileInfo;;

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

const std::string createPath(const std::string& path, const ServerConf& serverConf, const std::string method)
{
	std::map<std::string, Location> locationMap = serverConf.getLocation();
	std::string	returnPath;
	struct stat	s;
	bool		getFlag = false;

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++) {
		const	Location& loc = iter->second;
		const	std::vector<std::string>& methods = loc.getAllowMethods();
		for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++) {
			std::string cmp = *it;
			if (!cmp.compare(method)) {
				getFlag = true;
				break;
			}
		}
		if (getFlag) {
			if (loc.getRoot().empty())
				returnPath = serverConf.getRoot() + path;
			else
				returnPath = loc.getRoot() + path;
			if (stat(returnPath.c_str(), &s) == 0)
			{
				if (s.st_mode & S_IFDIR)
					returnPath += loc.getIndex();
				break;
			}
		}
		getFlag = false;
	}
	return returnPath;
}

std::string	processGetRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	HttpResponse	ret(serverConf);
	std::string alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";
	std::string vide = "";
	std::string localPath;

	if (request.getBody().size() > serverConf.getMaxBodySize())
		return (ret.generateResponse("413", vide, alive));
	//TODO send http error response instead
	
	std::string path = request.getPath();

	if (checkRedir(path, serverConf))
		return (ret.generateResponse("302", path, alive));

	if (DEBUG)
		std::cout << GREEN << "GET: PATH IS: " << path << RESET << std::endl;

	localPath = createPath(path, serverConf, "GET");

	if (DEBUG)
		std::cout << GREEN << "GET: Created Local Path: " << localPath << RESET << std::endl;

	if (!fileExists(localPath))
	{
		localPath = serverConf.getErrorPage();
		std::cerr << RED << "GET: ERROR: File not found: " << localPath << RESET << std::endl;
		return (ret.generateResponse("404", localPath, alive));
	}
	if (!hasAccess(localPath))
	{
		localPath = serverConf.getErrorPage();
		std::cerr << RED << "GET: ERROR: Access denied to file: " << localPath << RESET << std::endl;
		return (ret.generateResponse("403", localPath, alive));
	}
	return (ret.generateResponse("200", localPath , alive));
}
