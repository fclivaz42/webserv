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

std::string CGIExec::getCgiContentType() const
{
	std::istringstream headerStream(_header); // Crée flux à partir de _header pour lire ligne par ligne
	std::string line;

	while (std::getline(headerStream, line))
	{
		//std::cout << "je print la ligne" << line << std::endl;
		//std::cout << "" << std::endl;
		if (line.find("Content-Type") != std::string::npos)
		{
			size_t startPos = line.find(":") + 1; // Trouvez la position du début du type de contenu
			// Supprimez les espaces blancs au début
			while (startPos < line.length() && isspace(line[startPos]))
			{
				startPos++;
			}
			return line.substr(startPos); // Retournez le type de contenu
		}
	}
	return ""; // Retournez une chaîne vide si le type de contenu n'est pas trouvé
}

int CGIExec::execute(void){
	int pid;
	int fd[2];
	std::string lol = _request.getLoc().getFastcgiPass();
	const char *args[] = {lol.c_str(), _request.getCreatedPath().c_str(), NULL};
	int exitStatus = 0;

	std::cout << "------------ARG0 : " << args[0] << std::endl;
	if(pipe(fd) == -1)
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
	if(pid == 0)
	{
		launchChild(fd, args);
	}
	else
	{
		exitStatus = launchParent(fd, pid);
	}
	return(exitStatus);
}

int CGIExec::launchChild(int *fd, const char** args)
{
		const char *envp[] = {
    "GATEWAY_INTERFACE=CGI/1.1", 
    "REQUEST_METHOD=GET", 
    "QUERY_STRING=nbr1=1&operation=%2B&nbr2=1",
    NULL
	};
	std::cout << "ARG0 : " << args[0] << std::endl;
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	
	execve(args[0], const_cast<char**>(args), const_cast<char**>(envp));
	perror("execve");
	exit(-1);
}

int CGIExec::launchParent(int *fd, int pid)
{
	int status;
	int reading = 0;
	char tmp[BUFFERSIZE];
	int exitStatus;

	waitpid(pid, &status, 0);
	if(WIFEXITED(status))
	{
		exitStatus = WEXITSTATUS(status); // tout c est bien passe, enfant a quitte meme si excve a echoue
		std::cout << exitStatus << std::endl;
		if (exitStatus != 0)
			return(500);
	}
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	std::string buf;
	do
	{
		memset(tmp, 0, sizeof(tmp));
		ssize_t reading = read(fd[0], tmp, sizeof(tmp) - 1); // Utilisez sizeof(tmp) pour éviter de lire au-delà de la taille du tampon
		if (reading > 0)
		{
			buf.append(tmp, reading);
		}
	}
	while (reading > 0);
	findHeadAndBody(buf);
	close(fd[0]);

	return (exitStatus);
}

int CGIExec::findHeadAndBody(std::string buf)
{
	size_t headerEndPos = buf.find("\n\n");
	if (headerEndPos != std::string::npos)
	{
		_header = buf.substr(0, headerEndPos);
		_body = buf.substr(headerEndPos + 2); // +2 pour passer \n\n
	}
	else
	{
		_body = buf;
	}
	return(0);
}