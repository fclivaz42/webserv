#include "Network/ConnectManager.hpp"

/*
		handleClient launches the actual request after HTTPRequest has been parsed.
*/

const std::string	ConnectManager::handleClient(HTTPRequest& request)
{
	request.createPath();
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::cout << "REQUEST : " << request.getPath() << std::endl;

	if (headers["Expect"] == "100-continue") {
		if (request.getContentLength() <= request.getSConf().getMaxBodySize())
			return HTTPResponse::generateResponse(100, "", request.isKeepAlive(), request);
		else
			HTTPResponse::generateResponse(417, request.getSConf().getErrorPath(), "Connection: close", request);
	}
	else if (request.getContentLength() > request.getSConf().getMaxBodySize())
			HTTPResponse::generateResponse(413, request.getSConf().getErrorPath(), request.isKeepAlive(), request);
	else if (request.getMethod() == "GET")
		return processGetRequest(request);
	else if (request.getMethod() == "POST")
		return processPostRequest(request);
	else if (request.getMethod() == "DELETE")
		return processDeleteRequest(request);
	return HTTPResponse::generateResponse(500, "", request.isKeepAlive(), request);
}

void	ConnectManager::writeToClient(const std::string& response, int clientFd)
{
	ssize_t bytesWritten = write(clientFd, response.c_str(), response.length());
	if (bytesWritten == -1)
		std::cerr << RED << "ERROR:" << RESET << " write() failure.";
	else if (bytesWritten != static_cast<ssize_t>(response.length()))
		std::cerr << ORANGE << "WARN:" << RESET << " Failure to write entire response.";
	close(clientFd);
}

/*
	initializeRequest va generer les serverConf, Location et method/path/version pour avoir une HTTPRequest prete.
*/

const ServerConf&	ConnectManager::findSconfFromHost(std::map<std::string, std::string>& headers)
{
	std::string			fnbr;
	
	if (headers.find("Host") != headers.end()) {
		for (int i = 0; i < _serverList.getAmountOfServers(); i++) {
			const std::vector<ushort>& cPorts= _serverList.getServConf(i).getPort();
			for (std::vector<ushort>::const_iterator it = cPorts.begin(); it != cPorts.end(); it++) {
				std::stringstream	nbr;
				nbr << *it;
				fnbr = _serverList.getServConf(i).getServerName() + ":" + nbr.str();
				if (headers["Host"] == fnbr)
					return _serverList.getServConf(i);
			}
		}
	}
	return _serverList.getServConf(0);
}

const Location&	ConnectManager::findLocationFromSConf(const ServerConf& sConf, const std::string& path)
{
	const std::map<std::string, Location>&	locationMap = sConf.getLocation();
	std::string						locReq;
	size_t							pos;

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++) {
		locReq = iter->second.getPath();
		pos = -1;
		while (!(locReq.c_str()[++pos] == 0 || path.c_str()[pos] == 0))
			if (locReq.c_str()[pos] != path.c_str()[pos])
				break;
		if (locReq.c_str()[pos] == 0 && (path.c_str()[pos] == 0 || path.c_str()[pos] == '/'))
			return iter->second;
	}

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++)
		if (iter->second.isDefault())
			return iter->second;
	return locationMap.begin()->second;
}

void	ConnectManager::initializeRequest(int clientFd, const std::string& message, std::map<int, HTTPRequest*>& fdRequestMap)
{
	std::map<std::string, std::string>	headers;
	std::map<std::string, std::string>	attribs;
	std::string	line, key, value, body;
	const std::string&	alive ="Connection: close\r\n";
	size_t		pos, delim, tmp;

	fdRequestMap[clientFd] = NULL;
	delim = message.find("\r\n") + 2;
	std::istringstream requestLine(message.substr(0, delim - 2));
	requestLine >> attribs["method"] >> attribs["path"] >> attribs["version"];

	if (DEBUG) {
		std::stringstream	stRequest(message.substr(0, message.find_first_not_of(PRINTABLES) - 1));
		std::string			prRequest;
		std::cout << "\n┌────────── NEW REQUEST ──────────\n";
		while (std::getline(stRequest, prRequest))
			if (stRequest.peek() != EOF)
				std::cout << "│ " << prRequest << std::endl;
	}

	attribs["path"] = urlDecode(attribs["path"]);

	if (DEBUG) {
		std::cout << "├────────── REQUEST METADATA ──────────\n";
		std::cout << "│ Method: " << attribs["method"] << "" << std:: endl;
		std::cout << "│ Path: " << attribs["path"] << std:: endl;
		std::cout << "│ Version: " << attribs["version"] << "\n└────────── END REQUEST  ──────────\n";
	}

	while (delim < message.length())
	{
		tmp = message.find("\r\n", delim) + 2;
		if (delim >= message.length() || tmp >= message.length())
			break;
		line = message.substr(delim, tmp - delim - 1);
		delim = tmp;
		pos = line.find(":");
		if (pos != std::string::npos)
		{
			key = trim(line.substr(0, pos));
			value = trim(line.substr(pos + 1, line.length() - (pos + 1) - 1));
			if (key != "Content-Type")
				headers[key] = value;
			else if (value == "application/x-www-form-urlencoded")
				headers[key] = value;
			else if (value.find("multipart") != std::string::npos)
			{
				headers["boundary"] = value.substr(value.find("boundary=") + 9);
				headers[key] = value.substr(0, value.find(';'));
			}
		}
		else if (line == "\r")
			if (headers["Content-Type"] == "application/x-www-form-urlencoded") {
				body = message.substr(delim);
				break;
			}
			else
				continue;
		else if (line.find(headers["boundary"]) != std::string::npos)
		{
			body = message.substr(delim);
			break ;
		}
		else
			continue;
	}
/*
	if (headers.find("Referer") != headers.end()) {
    	size_t pos = headers.find("Referer")->second.find("?");
    	if (pos != std::string::npos) {
       		std::string name = headers.find("Referer")->second;
        	std::string _query = name.substr(pos + 1);
			size_t lastPos = name.find_last_of("/", pos);
			if (lastPos != std::string::npos)
            	attribs["fileName"] = name.substr(lastPos + 1, pos - lastPos - 1);
        	std::cout << "QUERY: " << _query << std::endl;
			std::cout << "FILE: " << _fileName << std::endl;
    	}
	}
*/


	const ServerConf& sConf = findSconfFromHost(headers);
	const Location&	loc = findLocationFromSConf(sConf, attribs["path"]);

	fdRequestMap[clientFd] = new HTTPRequest(attribs["method"], attribs["path"], attribs["version"], headers, sConf, loc);

	if (attribs["method"] != "GET" && attribs["method"] != "POST" && attribs["method"] != "DELETE")
		HTTPResponse::generateResponse(405, "GET, POST, DELETE", alive, *fdRequestMap[clientFd]);

	if (attribs["path"].empty() || attribs["path"][0] != '/')
		HTTPResponse::generateResponse(400, "", alive, *fdRequestMap[clientFd]);

	if (attribs["version"] != "HTTP/1.1" && attribs["version"] != "HTTP/1.0")
		HTTPResponse::generateResponse(505, "", alive, *fdRequestMap[clientFd]);

	if (attribs["method"] == "POST")
	{
		if (headers.find("Content-Length") == headers.end())
			HTTPResponse::generateResponse(411, "", alive, *fdRequestMap[clientFd]);
		if (headers.find("Content-Type") == headers.end())
			HTTPResponse::generateResponse(415, "", alive, *fdRequestMap[clientFd]);
	}
		char	*ptr;
		long	testsize = strtol(headers["Content-Length"].c_str(), &ptr, 10);
		if (testsize < 0 || ptr[0] != 0)
			HTTPResponse::generateResponse(418, "", alive, *fdRequestMap[clientFd]);

		fdRequestMap[clientFd]->setBodySize(strtoul(headers["Content-Length"].c_str(), NULL, 10));
}
