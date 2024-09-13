#include "Requests/HttpRequest.hpp"
#include <algorithm>
#include <string>

HttpRequest::HttpRequest() : method(""), path(""), version(""), body("")
{}

HttpRequest::HttpRequest(const std::string& request)
{
	parseRequest(request);
}

/* COPY CONSTRUCTOR */
HttpRequest::HttpRequest(HttpRequest const &copy) : method(copy.method), path(copy.path), version(copy.version), headers(copy.headers), body(copy.body)
{}

/* DESTRUCTOR */
HttpRequest::~HttpRequest()
{}

/* SURCHARGED OPERATORS */
HttpRequest &HttpRequest::operator=(HttpRequest const &rhs)
{
	if (this != &rhs)
	{
		method = rhs.method;
		path = rhs.path;
		version = rhs.version;
		headers = rhs.headers;
		body = rhs.body;
	}
	return (*this);
}

std::string	HttpRequest::getMethod() const
{
	return (method);
}

std::string	HttpRequest::getPath() const
{
	return (path);
}

std::string	HttpRequest::getVersion() const
{
	return (version);
}

std::map<std::string, std::string>	HttpRequest::getHeaders() const
{
	return (headers);
}

std::string	HttpRequest::getBody() const
{
	return (body);
}

std::string	trim(const std::string& str)
{
	size_t	start = str.find_first_not_of(" \t");

	if (start == std::string::npos)
		return ("");
	size_t end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

void	HttpRequest::parseRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string		line;

	if (DEBUG == 0) {
		std::stringstream	stRequest(request);
		std::string			prRequest;
		std::cout << "\n┌────────── NEW REQUEST ──────────\n";
		while (std::getline(stRequest, prRequest))
			if (stRequest.peek() != EOF)
				std::cout << "│ " << prRequest << std::endl;
	}
	if (!std::getline(iss, line) || line.empty())
	{
		std::cerr << ORANGE << "Waiting for request . . ." << RESET << std::endl;
		return ;
	}
	std::istringstream requestLine(line);
	requestLine >> method >> path >> version;

	if (DEBUG == 0) {
		std::cout << "├────────── REQUEST METADATA ──────────\n";
		std::cout << "│ method: " << "" << method << "" << std:: endl;
		std::cout << "│ path: " << path << std:: endl;
		std::cout << "│ version: " << version << "\n└────────── END REQUEST  ──────────\n\n";
	}
	//TODO throw real error response
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw std::runtime_error("ERROR: Request not allowed");

	//TODO search url in location path
	if (path.empty() || path[0] != '/')
		throw std::runtime_error("ERROR: Invalid path");

	if (version != "HTTP/1.1" && version != "HTTP/1.0")
		throw std::runtime_error("ERROR: Unsupported HTTP version");

	while (std::getline(iss, line) && line != "\r" && !line.empty())
	{
		std::string::size_type pos = line.find(":");
		if (pos != std::string::npos)
		{
			std::string key = trim(line.substr(0, pos));
			std::string value = trim(line.substr(pos + 1));
			headers[key] = value;
		}
		else
			throw std::runtime_error("ERROR: Invalid headers");
	}

	std::getline(iss, line);

	if (version == "HTTP/1.1" && headers.find("Host") == headers.end())
		throw std::runtime_error("ERROR: Missing host");

	if (method == "POST" && headers.find("Content-Length") != headers.end())
	{
		std::istringstream contentLenStream(headers["Content-Length"]);
		size_t contentLen = 0;
		contentLenStream >> contentLen;

		if (contentLen > 0)
		{
			body.resize(contentLen);
			iss.read(&body[0], contentLen);
			if (body.size() != contentLen)
				std::cerr << "ERROR body size does not match content Len" << std::endl;
			else
				std::cout << "SUCCESS: Body Read" << body << std::endl;
			if (DEBUG == 0)
			{
				std::cout << "\n├────────── REQUEST BODY ──────────\n";
				std::cout << body << "\n└────────── END BODY ──────────\n";
			}
		}
		specialPostParsing();
	}
}

void 		HttpRequest::specialPostParsing()
{
	if (headers.find("Content-Type") != headers.end())
	{
		std::string contentType = headers["Content-Type"];
		if (contentType.find("multipart/form-data") != std::string::npos)
		{
			std::string boundary = getBoundary(contentType);
			parseMultiPartBody(body, boundary);
		}
	}
}

std::string 	HttpRequest::getBoundary(const std::string& contentType)
{
	std::string boundary = "";
	size_t pos = contentType.find("boundary");
	if (pos != std::string::npos)
		boundary = "--" + contentType.substr(pos + 9);
	return (boundary);
}

void 		HttpRequest::parseMultiPartBody(const std::string& body, const std::string& boundary)
{
	size_t pos = 0;
	std::string del = boundary + "\r\n";

	while ((pos = body.find(del)) != std::string::npos)
	{
		size_t endPart = body.find(boundary, pos + del.length());
		std::string part = body.substr(pos + del.length(), endPart - (pos + del.length()));

		std::istringstream partStream(part);
		std::string partHeader;
		std::getline(partStream, partHeader);

		if (partHeader.find("Content-Disposition:") != std::string::npos)
		{
			size_t filenamePos = partHeader.find("filename=");
			if (filenamePos != std::string::npos)
			{
				size_t fileStart = part.find("\r\n\r\n", filenamePos) + 4;
				size_t fileEnd = part.rfind("\r\n");
				std::string fileData = part.substr(fileStart, fileEnd - fileStart);
				std::ofstream outFile("uploaded_file.jpeg", std::ios::binary);
				outFile.write(fileData.c_str(), fileData.size());
				outFile.close();
			}
		}
	}
}

bool		HttpRequest::isKeepAlive() const
{
	std::map<std::string, std::string>::const_iterator iter = headers.find("Connection");

	if (iter != headers.end())
	{
		std::string alive  = iter->second;
		std::transform(alive.begin(), alive.end(), alive.begin(), ::tolower);
		if (alive == "keep-alive")
			return (true);
		if (alive == "close")
			return (false);
	}
	if (version == "HTTP/1.1")
		return (true);
	return (false);

}
