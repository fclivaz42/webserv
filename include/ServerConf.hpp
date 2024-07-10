#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

#include "Servers.hpp"
#include <algorithm>
#include <map>

class ServerConf : public Servers {
	private:
		std::string _serverName;
		std::string	_port;
		std::string _root;
		std::string _index;
		std::map<std::string, std::string> _locations;

	public:
		/* CONSTRUCTORS */
		ServerConf();
		/* COPY CONSTRUCTORS */
		ServerConf(ServerConf const &cpy);
		/* DESTRUCTOR */
		~ServerConf();
		/* SURCHARGED OPERATORS */
		ServerConf	&operator=(ServerConf const &rhs);
		/* GETTERS */

		/* SETTERS */

		/* MEMBERS FUNCTIONS */

};

#endif