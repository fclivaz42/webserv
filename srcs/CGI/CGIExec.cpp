#include "CGI/CGIExec.hpp"
#include "Requests/Get.hpp"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

CGIExec::CGIExec(HTTPRequest &request) : _request(request){}

CGIExec::~CGIExec()
{}

std::string	CGIExec::getBody(void){
	return (this->_body);
}

std::string CGIExec::getHeader(void){
	return (this->_header);
}

std::string CGIExec::getCgiContentType() const
{
	std::istringstream headerStream(_header);
	std::string line;

	while (std::getline(headerStream, line))
	{
		if (line.find("Content-Type") != std::string::npos)
		{
			size_t startPos = line.find(":") + 1;
			while (startPos < line.length() && isspace(line[startPos]))
			{
				startPos++;
			}
			return line.substr(startPos);
		}
	}
	return "";
}

int CGIExec::execute(void){
	int pid;
	int fdoutput[2];
	int fdinput[2];
	std::string index = _request.getLoc().getFastcgiPass();
	const char *args[] = {index.c_str(), _request.getCreatedPath().c_str(), NULL};
	int exitStatus = 0;

	if(pipe(fdoutput) == -1)
	{
		perror("Pipe");
		exit(-1);
	}
	if(pipe(fdinput) == -1)
	{
		perror("Pipe");
		exit(-1);
	}

	pid = fork();

	if (pid == -1)
	{
		perror("Fork");
		exit(-1);
	}
	if (pid == 0)
		launchChild(fdoutput, fdinput, args);
	else
		exitStatus = launchParent(fdoutput, fdinput, pid);

	return(exitStatus);
}

int CGIExec::launchChild(int *fdoutput, int *fdinput, const char** args)
{
	std::string gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
    std::string requestMethod = "REQUEST_METHOD=" + _request.getMethod();
    std::string queryStringEnv = "QUERY_STRING=" + _request.getQuery();
	const char *envp[] = {
    	const_cast<char*>(gatewayInterface.c_str()),
        const_cast<char*>(requestMethod.c_str()),
        const_cast<char*>(queryStringEnv.c_str()),
    	NULL
	};

	close(fdoutput[0]);
	dup2(fdoutput[1], STDOUT_FILENO);
	close(fdoutput[1]);

	close(fdinput[1]);
	dup2(fdinput[0], STDIN_FILENO);
	close(fdinput[0]);

	execve(args[0], const_cast<char**>(args), const_cast<char**>(envp));
	perror("execve");
	exit(-1);
}

int CGIExec::launchParent(int *fdoutput, int *fdinput, int pid)
{
	int status;
	int reading = 0;
	char tmp[BUFFERSIZE];
	int exitStatus;

	close(fdinput[0]);
	ssize_t writing = write(fdinput[1], _request.getBody().c_str(), _request.getBody().size());
	(void) writing;
	close(fdinput[1]);

	waitpid(pid, &status, 0);
	if(WIFEXITED(status))
	{
		exitStatus = WEXITSTATUS(status);
		if (exitStatus != 0)
			return(500);
	}
	close(fdoutput[1]);
	std::string buf;
	do
	{
		memset(tmp, 0, sizeof(tmp));
		ssize_t reading = read(fdoutput[0], tmp, sizeof(tmp) - 1);
		{
			buf.append(tmp, reading);
		}
	}
	while (reading > 0);

	findHeadAndBody(buf);
	close(fdoutput[0]);

	return (exitStatus);
}

int CGIExec::findHeadAndBody(std::string buf)
{
	std::istringstream stream(buf);
    std::string line;
    bool headerEnded = false;

    while (std::getline(stream, line)) {
        if (line == "\r" || line == "") {
            headerEnded = true; 
            continue;
        }
        if (!headerEnded) {
            _header += line + "\r\n";
        } else {
            _body += line + "\n";
        }
    }

    if (_header.empty() && _body.find("Content-Type") != std::string::npos) {
        size_t pos = _body.find("\n");
        _header = _body.substr(0, pos);
        _body = _body.substr(pos + 1);
    }
	if (DEBUG){
		std::cout << "Headers CGI: " << _header << std::endl;
		std::cout << "Body CGI: " << _body << std::endl;
	}

    return 0;
}
