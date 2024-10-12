#include "Requests/HTTPResponse.hpp"
#include "Requests/Get.hpp"

HTTPResponse::HTTPResponse(const ServerConf& serverConf) : serverConf(serverConf)
{}

std::string	HTTPResponse::generateResponse(const std::string& statusCode, std::string& path, const std::string &alive)
{
	std::cout << "IS CONNECTION ALIVE........? " << alive << std::endl;
	if (statusCode == "204")
			return ("HTTP/1.1 204 No Content\r\n" + alive + "\r\n");
	
	if (statusCode[0] == '4'){
		path = loadErrorPage(statusCode);
		std::cout << "Calling loadErrorPage for status code: " << statusCode << std::endl;
	}

	std::string content = ConnectManager::readFile(path);
	std::string contentType = getContentType(path);

	if (statusCode == "200")
			return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
	else if (statusCode == "302")
			return ("HTTP/1.1 302 Found\r\nLocation: " + path + "\r\nConnection: close\r\n\r\n");
	else if (statusCode == "403")
			return ("HTTP/1.1 403 Forbidden\r\n" + contentType + "\r\n" + alive + "\r\n" + content);
	else if (statusCode == "404")
			return ("HTTP/1.1 404 Not Found\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
	else if (statusCode == "413")
		return ("HTTP/1.1 413 Payload Too Large\r\nContent-Type: " + contentType + "\r\n" + alive + "\r\n" + content);
	else
		return ("HTTP/1.1 500 Internal Server Error\r\n\r\n");
}

std::string		HTTPResponse::loadErrorPage(const std::string& errorCode) const{
	std::string	errorPath = serverConf.getErrorPage();

	errorPath.replace(16, 3, errorCode);
	return (errorPath);
}

std::string		HTTPResponse::getContentType(const std::string& path)
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

std::string		HTTPResponse::readFileContent(const std::string& path) const
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cerr << "ERROR: Unable to open file: " << path << std::endl;
		return ("");
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	return (oss.str());
}

