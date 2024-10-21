#include "Requests/Get.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

// TODO:: database delete

std::string processDeleteRequest(const HTTPRequest& request, const ServerConf& serverConf)
{
	std::cout << "PATH BEFORE: " << request.getPath() << std::endl;
	std::string	delPath = HTTPRequest::createPath(request.getPath(), serverConf, "DELETE", false);
	struct stat	s;
	std::cout << ORANGE << "Requested DELETE path: " << delPath << RESET << std::endl;
	

	if (access(delPath.c_str(), F_OK) != 0) {
		std::cerr << RED << "DELETE: File not found: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(404, "", request.isKeepAlive(), serverConf);
	}
	if (access(delPath.c_str(), W_OK) != 0) {
		std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(403, "", request.isKeepAlive(), serverConf);
	}
	if (stat(delPath.c_str(), &s) != 0) {
		std::cerr << RED << "DELETE: Failed to delete file: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(500, delPath, request.isKeepAlive(), serverConf);
	}
	if (s.st_mode & S_IFDIR) {
		std::cerr << RED << "DELETE: No permission to delete directories: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(403, "", request.isKeepAlive(), serverConf);
	}
	else if (remove(delPath.c_str()) == 0) {
		std::cout << GREEN << "DELETE: File successfully deleted: " << delPath << RESET << std::endl;
	}
	else {
		std::cerr << RED << "DELETE: Failed to delete file: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(500, delPath, request.isKeepAlive(), serverConf);
	}
	return HTTPResponse::generateResponse(204, delPath, request.isKeepAlive(), serverConf);
}
