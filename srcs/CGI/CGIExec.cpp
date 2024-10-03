#include "CGI/CGIExec.hpp"
#include "Requests/Get.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>


CGIExec::CGIExec(const std::string& cgiPath, const std::map<std::string, std::string>& env)
	: _cgiPath(cgiPath), _env(env) {}

CGIExec::~CGIExec()
{}

void	CGIExec::setupEnvVars()
{
	for (std::map<std::string, std::string>::const_iterator iter = _env.begin(); iter != _env.end(); iter++)
		setenv(iter->first.c_str(), iter->second.c_str(), 1);
}

std::string	CGIExec::runCGIProcess(const std::string& input)
{
	int	pipefd[2];
	int	inputPipe[2];

	if (pipe(pipefd) == -1) {
			std::cerr << "Error creating pipe for CGI output: " << strerror(errno) << std::endl;
			return "Internal Server Error";
		}
	if (pipe(inputPipe) == -1) {
			std::cerr << "Error creating pipe for CGI input: " << strerror(errno) << std::endl;
			return "Internal Server Error";
		}

	pid_t	pid = fork();
	
	if (pid == -1) {
		std::cerr << "Fork failed: " << strerror(errno) << std::endl;
		return "Internal Server Error";
	}

	if (pid == 0)
	{
		//dup2(pipefd[1], STDOUT_FILENO);
		//close(pipefd[0]);
		//close(pipefd[1]);

		//dup2(inputPipe[0], STDIN_FILENO);
		//close(inputPipe[1]);
		//close(inputPipe[0]);
		
		execl(_cgiPath.c_str(), _cgiPath.c_str(), NULL);
		std::cout << "COUOCOUUUUUU " << std::endl;
		exit(1);
	}
	else
	{
		close(pipefd[1]);
		close(inputPipe[0]);

		if (write(inputPipe[1], input.c_str(), input.size()) == -1) {
			std::cerr << "Error writing to CGI input pipe: " << strerror(errno) << std::endl;
			return "Internal Server Error";
		}

		close(inputPipe[1]);

		char buffer[1024];
		std::string output;
		ssize_t nbytes;

		while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, nbytes);
		close(pipefd[0]);

		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			std::cerr << "CGI script exited with error status: " << WEXITSTATUS(status) << std::endl;
			return "Internal Server Error";
		}

		return (output);
	}
}

std::string	CGIExec::execute(const std::string &input)
{
	setupEnvVars();
	return (runCGIProcess(input));
}
