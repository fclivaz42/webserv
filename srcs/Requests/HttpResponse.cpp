#include "Requests/HttpResponse.hpp"
#include "Requests/Get.hpp"

HttpResponse::HttpResponse(const ServerConf& serverConf) : serverConf(serverConf)
{}

const std::string	HttpResponse::generateResponse(int statusCode, const std::string& path) const
{
	std::string content = SocketManager::readFile(path);
	std::string contentType = getContentType(path);

	switch (statusCode) {
		case (200):
			return ("HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n\r\n" + content);
		case (302):
			return ("HTTP/1.1 302 Found\r\nLocation: " + path + "\r\nConnection: close\r\n\r\n");
		case (403):
			return ("HTTP/1.1 403 Forbidden\r\n" + getErrorPage("403") + "\r\n");
		case (404):
			std::cout << RED << content << RESET << std::endl;
			std::cout << RED << contentType << RESET << std::endl;
			return ("HTTP/1.1 404 Not Found\r\nContent-Type: " + contentType +  "\r\n\r\n" + content);
		case (413):
			return ("HTTP/1.1 413 Payload Too Large\r\nConnection: close\r\n\r\n");
		default:
			return ("HTTP/1.1 500 Internal Server Error\r\n\r\n");
	}
}

std::string		HttpResponse::getErrorPage(const std::string& errorCode) const
{
	std::string	errorPath;
	(void) errorCode;
	//TODO create this muddafucka
	//errorPath = serverConf.getErrorPage(errorCode);
	if (errorPath.empty())
		errorPath = "/public/errors/404.html";
	return (readFileContent(errorPath));
}

std::string		HttpResponse::getContentType(const std::string& path) const
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

std::string		HttpResponse::readFileContent(const std::string& path) const
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

