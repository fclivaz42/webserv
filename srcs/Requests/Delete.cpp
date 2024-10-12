#include "Requests/Get.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HTTPRequest.hpp"
#include "Requests/HTTPResponse.hpp"

bool hasDeletePermissions(const std::string& path){
    return (access(path.c_str(), W_OK) == 0);
}

std::string processDeleteRequest(const HTTPRequest& request, const ServerConf& serverConf){
    std::cout << "PATH BEFORE: " << request.getPath() << std::endl;
	std::string delPath = createPath(request.getPath(), serverConf, "DELETE", "OMG SWAG");
    std::cout << ORANGE << "Requested DELETE path: " << delPath << RESET << std::endl;
	
    HTTPResponse response(serverConf);
	std::string errorPage = serverConf.getErrorPage();

	if (!fileExists(delPath)) {
        std::cerr << RED << "DELETE: File not found: " << delPath << RESET << std::endl;
        return (response.generateResponse("404", errorPage, request.isKeepAlive()));
    }
	else if (delPath.find(serverConf.getRoot() + "/delete/") == 0){
		if (!hasDeletePermissions(delPath)){
        	std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("403", errorPage, request.isKeepAlive()));
    	}
		else if (remove(delPath.c_str()) == 0){
        	std::cout << GREEN << "DELETE: File successfully deleted: " << delPath << RESET << std::endl;
        	return (response.generateResponse("204", delPath, request.isKeepAlive()));
    	} 
		else{
        	std::cerr << RED << "DELETE: Failed to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("500", delPath, request.isKeepAlive()));
    	}
	}
	else{
	std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("403", errorPage, request.isKeepAlive()));
	}
}
