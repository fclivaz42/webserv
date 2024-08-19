#ifndef SERVERS_HPP
# define SERVERS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "ServerConf.hpp"


class Servers{
	private:
		std::vector<ServerConf> _servConf;

	public: 
		/* CONSTRUCTORS */
		Servers(const std::string &confFile);
		/* COPY CONSTRUCTOR */
		Servers(Servers const &cpy);
		/* DESTRUCTOR */
		~Servers();
		/* SURCHARGED OPERATORS */
		Servers &operator=(Servers const &rhs);
		/* GETTERS */
		std::vector<ServerConf> getServConf(void) const;
		/* SETTERS */
		
		/* MEMBERS FUNCTIONS */
		void	loadConfig(const std::ifstream confFile);
		void	parseConf(void);
};

#endif
