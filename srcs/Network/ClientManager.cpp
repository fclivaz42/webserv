#include "Network/ConnectManager.hpp"
#include "CGI/CGIExec.hpp"
#include "Parsing/ServerConf.hpp"

bool	isCGI(HTTPRequest &request){
	
	
	if (request.getLoc().getFastcgiIndex().size() == 0)
		return (false);
	if (request.getCreatedPath().rfind(request.getLoc().getFastcgiIndex()) == request.getCreatedPath().size() - request.getLoc().getFastcgiIndex().size())
		return (true);
	else
		return (false);
}

/*
		handleClient launches the actual request after HTTPRequest has been parsed.
*/

const std::string	ConnectManager::handleClient(HTTPRequest& request)
{
	request.createPath();
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::cout << "REQUEST : " << request.getPath() << std::endl;

	if (request.getContentLength() > request.getSConf().getMaxBodySize())
			HTTPResponse::generateResponse(413, request.getSConf().getErrorPath(), request.isKeepAlive(), request);
	if (isCGI(request) == true) {
		CGIExec cgi(request);
		int status = cgi.execute();

		if (status == 500)
			HTTPResponse::generateResponse(500, request.getSConf().getErrorPath(), request.isKeepAlive(), request);
		else{
			std::string contentType = cgi.getCgiContentType();
			std::string body = cgi.getBody();
			std::string test = cgi.getHeader();
			std::string response = "HTTP/1.1 200 OK\r\n" + cgi.getHeader();
			std::stringstream truc;
			
			truc << cgi.getBody().size();
			response += "Content-Length: " + truc.str() + "\r\n\r\n";
			response += cgi.getBody();

			if (DEBUG)
				std::cout << "Final Response:\n" << response << std::endl;
			
			return (response);
		}
		
	}
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

void	ConnectManager::redirectPath(const ServerConf& sConf,const Location& loc, std::string& path)
{
	const std::map<std::string, Location>&	locationMap = sConf.getLocation();
	std::string						locReq;
	Location						oldLoc;
	size_t							pos;

	for (std::map<std::string, Location>::const_iterator iter = locationMap.begin(); iter != locationMap.end(); iter++) {
		locReq = iter->second.getPath();
		pos = -1;
		while (!(locReq.c_str()[++pos] == 0 || path.c_str()[pos] == 0))
			if (locReq.c_str()[pos] != path.c_str()[pos])
				break;
		if (locReq.c_str()[pos] == 0 && (path.c_str()[pos] == 0 || path.c_str()[pos] == '/')) {
			oldLoc = iter->second;
		}
	}

	if (oldLoc.getPath() != loc.getPath()) {
		std::cout << "switching paths...\n";
		path.replace(path.find(oldLoc.getPath()), oldLoc.getPath().length(), loc.getPath());
	}
}

/*
	initializeRequest va generer les serverConf, Location et method/path/version pour avoir une HTTPRequest prete.
*/

const ServerConf&	ConnectManager::findSconfFromHost(std::map<std::string, std::string>& headers)
{
	std::string	fnbr;
	
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
		if (locReq.c_str()[pos] == 0 && (path.c_str()[pos] == 0 || path.c_str()[pos] == '/')) {
			if (iter->second.getReturnURL().empty())
				return iter->second;
			else if (iter->second.getReturnURL().find("http://") != std::string::npos || iter->second.getReturnURL().find("https://") != std::string::npos)
				return iter->second;
			else
				return findLocationFromSConf(sConf, iter->second.getReturnURL());
		}
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
	const std::string&	alive = "Connection: close\r\n";
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

	pos = attribs["path"].find("?");
	if (pos != std::string::npos) {
		attribs["query"] = attribs["path"].substr(pos + 1);
		attribs["path"] = attribs["path"].substr(0, pos);
	}
	else
		attribs["query"] = "";

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
				attribs["body"] = message.substr(delim);
				break;
			}
			else
				continue;
		else if (line.find(headers["boundary"]) != std::string::npos)
		{
			attribs["body"] = message.substr(delim);
			break ;
		}
		else
			continue;
	}

	const ServerConf& sConf = findSconfFromHost(headers);
	const Location&	loc = findLocationFromSConf(sConf, attribs["path"]);
	redirectPath(sConf, loc, attribs["path"]);

	fdRequestMap[clientFd] = new HTTPRequest(attribs, headers, sConf, loc);

	if (attribs["method"] != "GET" && attribs["method"] != "POST" && attribs["method"] != "DELETE")
		HTTPResponse::generateResponse(405, "GET, POST, DELETE", alive, *fdRequestMap[clientFd]);

	if (attribs["Expect"] == "100 Continue")
		HTTPResponse::generateResponse(400, "", alive, *fdRequestMap[clientFd]);

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
