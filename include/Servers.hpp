#ifndef SERVERS_HPP
# define SERVERS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "ServerConf.hpp"
#include "Location.hpp"

class ServerConf;
class Location;

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
		void									loadConfig(const std::string &confFile);
		std::pair <std::string, std::string>	pairConf(std::string line);
		void									setConf(std::pair<std::string, std::string> keyValue, ServerConf &current);
		void									setConfLoc(std::pair<std::string, std::string> keyValue, Location &current);
		void 									printConfigs(void) const;
		int										checkValue(void);
		
};

#endif
