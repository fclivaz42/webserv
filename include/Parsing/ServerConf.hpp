#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

#include "Servers.hpp"
#include <algorithm>
#include <map>
#include <cctype>

class Location;

class ServerConf{
	private:
		std::string 					_serverName;
		std::vector<unsigned short>		_port;
		std::string 					_root;
		std::string 					_index;
		std::size_t						_maxBodySize;
		std::string 					_errorPage;
		std::string						_ipAddr;
		std::map<std::string, Location> _location;

	public:
		/* CONSTRUCTORS */
		ServerConf();
		ServerConf(const std::string& configString);

		/* COPY CONSTRUCTORS */
		ServerConf(ServerConf const &cpy);

		/* DESTRUCTOR */
		~ServerConf();

		/* SURCHARGED OPERATORS */
		ServerConf	&operator=(ServerConf const &rhs);

		/* GETTERS */
		std::string 					getServerName(void) const;
		std::vector<unsigned short>		getPort(void) const;
		std::string 					getRoot(void) const;
		std::string						getIndex(void) const;
		std::size_t						getMaxBodySize(void) const;
		std::string						getErrorPage(void) const;
		std::string						getIpAddr(void)	const;
		std::map<std::string, Location>	getLocation(void) const;

		/* MEMBERS FUNCTIONS */
		void							print(void) const;
		void							checkAttribut(void) const;

		/* EXCEPTIONS */
		class InvalidServerConfException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid server configurtation format.\n";
				}
		};
		class InvalidPortException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid port.\n";
				}
		};
		class InvalidBodySizeException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid Max Body Size.\n";
				}
		};
		class MissingArgsException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Missing args in server configuration.\n";
				}
		};
		class InvalidRootPathException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid Root Path.\n";
				}
		};
		class InvalidErrorPageException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid error page path.\n";
				}
		};
		class UnexpectedEOFException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Unexpected End Of File.\n";
				}
		};
};

#endif
