#include "Requests/Post.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/Get.hpp"
#include "CGI/CGIExec.hpp"
#include <string>

std::string urlDecode(const std::string& str)
{
    std::string result;
    size_t length = str.length();

    for (size_t i = 0; i < length; ++i) {
        if (str[i] == '%') {
            if (i + 2 < length) {
                int value;
                std::istringstream is(str.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    result += static_cast<char>(value);
                    i += 2;
                }
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

const std::string	uploadRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string>	headers = request.getHeaders();
	std::string							body = request.getBody();
	std::string							fileName;

	if (DEBUG)
		std::cout << GREEN << "POST: Image being uploaded." << std::endl;
	fileName = body.substr(body.find("Content-Disposition"), body.find("Content-Type"));
	fileName = fileName.substr(fileName.find("filename=\"") + 10);
	fileName = fileName.substr(0, fileName.find_last_of("\""));
	std::string path = createPath("/" + fileName, serverConf, "POST");
	if (DEBUG)
		std::cout << "POST: Created path: " << path << RESET << std::endl;

	body = body.substr(body.find("\r\n\r\n") + 4);
	body = body.substr(0, body.find(headers["boundary"]) - 4);
	std::cout << "BODY SIZE: " << body.length() << std::endl;
	std::ofstream outFile(path.c_str(), std::ios::out | std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(body.c_str(), body.size());
		outFile.close();
	} else {
		std::cerr << "ERROR: Failed to open file for writing" << std::endl;
	}

	std::string response;

	HttpResponse res(serverConf);

	// Generate redirection header
	response += "HTTP/1.1 302 Found\r\n";
	response += "Location: /success.html\r\n";
	response += "Content-Type: text/html\r\n";

	// Construct Connection header
	std::string connectionHeader = request.isKeepAlive() ? "keep-alive" : "close";
	response += "Connection: " + connectionHeader + "\r\n";

	response += "\r\n"; // End of headers
//
	// Optionally, add content here if needed, e.g., an HTML message indicating the redirect
	response += "<html><body><p>Redirecting to <a href=\"/success.html\">success.html</a></p></body></html>";

	return response;
}
const std::string	formRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> formData;
	std::istringstream bodyStream(request.getBody());
	std::string keyValue, username, email, message, alive;

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
	alive = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";

	HttpResponse		res(serverConf);
	std::string			path(createPath(request.getPath() + ".html", serverConf, "POST")), line, response;
	std::stringstream	genRes(res.generateResponse("200", path, alive));
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


std::map<std::string, std::string> createCGIEnv(const HttpRequest& request)
{
    std::map<std::string, std::string> env;
    
    env["REQUEST_METHOD"] = "POST";
    env["CONTENT_TYPE"] = request.getHeaders().at("Content-Type");
    env["CONTENT_LENGTH"] = intToString(request.getBody().length());
    env["SCRIPT_NAME"] = request.getPath();
    env["REQUEST_URI"] = request.getPath();
	env["QUERY_STRING"] = "";
	env["PATH_INFO"] = request.getPath();
	env["PATH TRANSLATED"] = request.getPath();

    return (env);
}

const std::string	processPostRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	if (isCGIRequest(request.getPath()))
	{
		std::string cgiPath = "/cgi-bin/script.py";
        	std::map<std::string, std::string> env = createCGIEnv(request);
		CGIExec cgiExec(cgiPath, env);;
		return (cgiExec.execute(request.getBody()));
	}
	if (headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
		return (formRequest(request, serverConf));
	return (uploadRequest(request, serverConf));
}
