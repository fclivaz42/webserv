#ifndef CGIExec_HPP

# define CGIExec_HPP
#define BUFFERSIZE 20000

#include <string>
#include <map>
#include "Requests/HTTPRequest.hpp"

class CGIExec
{
	private:
		HTTPRequest		&_request;
		std::string		_header;
		std::string		_body;
	
	public:
		CGIExec(HTTPRequest &request);
		~CGIExec();
		int	execute();
		int launchChild(int *fd, const char** args);
		int launchParent(int *fd, int pid);
		int findHeadAndBody(std::string buf);
		std::string getCgiContentType() const;
		std::string getBody(void);

};

#endif