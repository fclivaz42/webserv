#ifndef CGIExec_HPP

# define CGIExec_HPP

#include <string>
#include <map>

class CGIExec
{
	public:
		CGIExec(const std::string& cgiPath, const std::map<std::string, std::string>& env);
		~CGIExec();
		std::string	execute(const std::string &input);

	private:
		std::string 				_cgiPath;
		std::map<std::string, std::string>	_env;
		void					setupEnvVars();
		std::string				runCGIProcess(const std::string& input);

};

#endif