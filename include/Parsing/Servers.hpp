#ifndef SERVERS_HPP
# define SERVERS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

class ServerConf;
class Location;

class Servers{
	private:
		std::vector<ServerConf> _servConf;

	public: 
		/* CONSTRUCTORS */
		Servers();
		Servers(const std::string &confFile);

		/* COPY CONSTRUCTOR */
		Servers(Servers const &cpy);

		/* DESTRUCTOR */
		~Servers();

		/* SURCHARGED OPERATORS */
		Servers &operator=(Servers const &rhs);

		/* GETTERS */
		ServerConf 				getServConf(int lequel) const;
		
		/* MEMBERS FUNCTIONS */
		void					loadConfig(const std::string &confFile);
		void 					printConfigs(void) const;
		bool					isConfigured(void) const;

		/* EXCEPTIONS */
		class InvalidServersException : public std::exception{
        public:
            virtual char const	*what(void) const throw();
		};
		
};

#endif
