#include "Requests/Get.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

bool hasDeletePermissions(const std::string& path){
	return (access(path.c_str(), W_OK) == 0);
}

// TODO:: fixer ce truc ptdr et checker que createpath retourne le bon truc

std::string processDeleteRequest(const HTTPRequest& request, const ServerConf& serverConf){
	std::cout << "PATH BEFORE: " << request.getPath() << std::endl;
	std::string delPath = HTTPRequest::createPath(request.getPath(), serverConf, "DELETE", "OMG SWAG");
	std::cout << ORANGE << "Requested DELETE path: " << delPath << RESET << std::endl;
	

	if (!fileExists(delPath)) {
		std::cerr << RED << "DELETE: File not found: " << delPath << RESET << std::endl;
		HTTPResponse::generateResponse(404, "", request.isKeepAlive(), serverConf);
	}
	else if (delPath.find(serverConf.getRoot() + "/delete/") == 0){
		if (!hasDeletePermissions(delPath)){
			std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
			HTTPResponse::generateResponse(403, "", request.isKeepAlive(), serverConf);
		}
		else if (remove(delPath.c_str()) == 0){
			std::cout << GREEN << "DELETE: File successfully deleted: " << delPath << RESET << std::endl;
		}
		else{
			std::cerr << RED << "DELETE: Failed to delete file: " << delPath << RESET << std::endl;
			HTTPResponse::generateResponse(500, delPath, request.isKeepAlive(), serverConf);
		}
	}
	return HTTPResponse::generateResponse(204, delPath, request.isKeepAlive(), serverConf);
}
