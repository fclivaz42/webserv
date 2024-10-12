#include "Requests/Post.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/Get.hpp"
#include "CGI/CGIExec.hpp"
#include <string>

std::string urlDecode(const std::string& str)
{
	std::string	result;
	size_t		length = str.length();

	for (size_t i = 0; i < length; ++i)
	{
		if (str[i] == '%')
		{
			if (i + 2 < length)
			{
				int value;
				std::istringstream is(str.substr(i + 1, 2));
				if (is >> std::hex >> value)
				{
					result += static_cast<char>(value);
					i += 2;
				}
			}
		}
		else if (str[i] == '+')
			result += ' ';
		else
			result += str[i];
	}
	return result;
}

const std::string	uploadRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	const std::string&					shift = request.getBody().str();
	std::string							fileName, line, path;
	std::string	response("HTTP/1.1 302 Found\r\nLocation: /success.html\r\nContent-Type: text/html\r\n");

	if (DEBUG)
		std::cout << GREEN << "POST: File being uploaded." << std::endl;
	fileName = shift.substr(shift.find("filename=\"") + 10);
	fileName = fileName.substr(0, fileName.find_first_of("\""));
	path = createPath("/" + fileName, serverConf, "POST", "Accept-Uploads");
	if (DEBUG)
		std::cout << "POST: Created path: " << path << RESET << std::endl;

	std::ofstream outFile(path.c_str(), std::ios::out | std::ios::binary);
	if (outFile.is_open()) {
		size_t pos = shift.find("\r\n\r\n") + 4;
		size_t pos2 = shift.find(headers["boundary"]) - pos - 4;
		outFile.write(shift.c_str() + pos, pos2);
		outFile.close();
	} else {
		std::cerr << "ERROR: Failed to open file for writing" << std::endl;
	}

	HTTPResponse res(serverConf);
	// Construct Connection header
	response += "Connection: " + request.isKeepAlive() + "\r\n\r\n";

	// Optionally, add content here if needed, e.g., an HTML message indicating the redirect
	response += "<html><body><p>Redirecting to <a href=\"/success.html\">success.html</a></p></body></html>";

	return response;
}

const std::string	formRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> formData;
	std::stringstream& bodyStream(request.getBody());
	std::string keyValue, username, email, message;

	if (DEBUG)
		std::cout << GREEN << "POST: Form received.\n";
	while (std::getline(bodyStream, keyValue, '&'))
	{
		size_t pos = keyValue.find('=');
		if (pos != std::string::npos)
		{
			std::string key = urlDecode(keyValue.substr(0, pos));
			std::string value = urlDecode(keyValue.substr(pos + 1));
			formData[key] = value;
		}
	}

	username = formData["name"];
	email = formData["email"];
	message = formData["message"];

	HTTPResponse		res(serverConf);
	std::string			path(createPath(request.getPath() + ".html", serverConf, "POST", "Accept-Uploads")), line, response;
	std::stringstream	genRes(res.generateResponse("200", path, request.isKeepAlive()));
	std::size_t			pos;

	while (std::getline(genRes, line))
	{
		pos = line.find("+username+");
		if (pos != std::string::npos)
			line.replace(pos, 10, username);
		pos = line.find("+email+");
		if (pos != std::string::npos)
			line.replace(pos, 7, email);
		pos = line.find("+message+");
		if (pos != std::string::npos)
			line.replace(pos, 9, message);

		response += line + '\n';
	}
	return (response);
}

bool		isCGIRequest(const std::string& path)
{
	std::string cgiPath = "/cgi-bin/";
	return (path.find(cgiPath) == 0);
}

std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


std::map<std::string, std::string> createCGIEnv(HTTPRequest& request)
{
    std::map<std::string, std::string> env;

    env["REQUEST_METHOD"] = "POST";
    env["CONTENT_TYPE"] = request.getHeaders().at("Content-Type");
    env["CONTENT_LENGTH"] = intToString(request.getBody().tellp());
    env["SCRIPT_NAME"] = request.getPath();
    env["REQUEST_URI"] = request.getPath();
	env["QUERY_STRING"] = "";
	env["PATH_INFO"] = request.getPath();
	env["PATH TRANSLATED"] = request.getPath();

    return (env);
}

const std::string	processPostRequest(HTTPRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	if (isCGIRequest(request.getPath()))
	{
		std::string cgiPath = "/cgi-bin/script.py";
        	std::map<std::string, std::string> env = createCGIEnv(request);
		CGIExec cgiExec(cgiPath, env);;
		return (cgiExec.execute(request.getBody().str()));
	}
	if (headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
		return (formRequest(request, serverConf));
	return (uploadRequest(request, serverConf));
}
