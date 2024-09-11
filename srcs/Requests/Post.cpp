#include "Requests/Post.hpp"
#include "Parsing/ServerConf.hpp"
#include "Requests/HttpRequest.hpp"
#include "Requests/HttpResponse.hpp"

std::string urlDecode(const std::string& str) {
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

const std::string	uploadRequest(const HttpRequest& request)
{
	std::cout << "WE HIT UPLOAD !!!!!!!!!!!!! \n\n\n";
	return "";
}

const std::string	processPostRequest(const HttpRequest& request, const ServerConf& serverConf)
{
	std::map<std::string, std::string> headers(request.getHeaders());

	if (headers["Content-Type"].find("multipart") != std::string::npos)
		return (uploadRequest(request));

	std::map<std::string, std::string> formData;
	std::istringstream bodyStream(request.getBody());
	std::string keyValue, username, email, message, alive;

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

	const HttpResponse	res(serverConf);
	return (res.generateResponse("200", "public/uploadFormReturn.html"));
}
