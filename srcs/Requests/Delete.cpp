#include "Requests/Get.hpp"
#include "Requests/Delete.hpp"
#include "Requests/HttpRequest.hpp"
#include "Requests/HttpResponse.hpp"

bool hasDeletePermissions(const std::string& path){
    return (access(path.c_str(), W_OK) == 0);
}

std::string processDeleteRequest(const HttpRequest& request, const ServerConf& serverConf){
    std::string delPath = request.getPath();
    std::cout << ORANGE << "Requested DELETE path: " << delPath << RESET << std::endl;

    delPath = updatePath(delPath);
    HttpResponse response(serverConf);
	std::string connectionHeader = request.isKeepAlive() ? "Connection: keep-alive\r\n" : "Connection: close\r\n";
	std::string errorPage = serverConf.getErrorPage();

	if (!fileExists(delPath)) {
        std::cerr << RED << "DELETE: File not found: " << delPath << RESET << std::endl;
        return (response.generateResponse("404", errorPage, connectionHeader));
    }
	else if (delPath.find(serverConf.getRoot() + "/delete/") == 0){
		if (!hasDeletePermissions(delPath)){
        	std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("403", errorPage, connectionHeader));
    	}
		else if (remove(delPath.c_str()) == 0){
        	std::cout << GREEN << "DELETE: File successfully deleted: " << delPath << RESET << std::endl;
        	return (response.generateResponse("204", delPath, connectionHeader));
    	} 
		else{
        	std::cerr << RED << "DELETE: Failed to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("500", delPath, connectionHeader));
    	}
	}
	else{
		std::cerr << RED << "DELETE: No permission to delete file: " << delPath << RESET << std::endl;
        	return (response.generateResponse("403", errorPage, connectionHeader));
	}
}
