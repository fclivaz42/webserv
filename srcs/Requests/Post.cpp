#include "Requests/Post.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/Get.hpp"

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
	if (DEBUG)
		std::cout << GREEN << "POST: Image being uploaded.\n";
	std::string	body = request.getBody();
	body = body.substr(body.find("filename="));
	body = body.substr(0, body.find_last_of('"'));
	body = body.erase(0, body.find_first_of('"') + 1);
	std::string path = createPath("/" + body, serverConf, "POST");
	std::cout << "POST: Created path: " << path << "\n";
	std::cout << "POST: DATA: " << request.getBody() << "\n";
	return "";
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

const std::string	processPostRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	if (headers["Content-Type"].find("multipart") != std::string::npos)
		return (uploadRequest(request, serverConf));
	return (formRequest(request, serverConf));
}
