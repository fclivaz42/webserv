#ifndef SERVER_CONF_HPP
# define SERVER_CONF_HPP

#include "Servers.hpp"
#include "Location.hpp"
#include <algorithm>
#include <map>

class ServerConf{
	private:
		std::string _serverName;
		std::string	_port;
		std::string _root;
		std::string _index;
		std::string _errorPage;
		std::map<std::string, Location> _location;

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
		std::string getServerName(void) const;
		std::string	getPort(void) const;
		std::string getRoot(void) const;
		std::string	getIndex(void) const;
		std::map<std::string, Location>	getLocation(void) const;

		/* SETTERS */
		void	setServerName(const std::string &serverName);
		void	setPort(const std::string &port);
		void	setRoot(const std::string &root);
		void	setIndex(const std::string &index);
		void	setLocation(const std::map<std::string, Location> &location);

		/* MEMBERS FUNCTIONS */

};

#endif
