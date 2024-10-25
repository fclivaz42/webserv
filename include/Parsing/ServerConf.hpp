#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

#include "Servers.hpp"
#include <algorithm>
#include <map>
#include <cctype>

class Location;

class ServerConf{
	private:
		std::string						_serverName;
		std::vector<unsigned short>		_port;
		std::string						_root;
		std::string						_index;
		std::size_t						_maxBodySize;
		std::string						_errorPath;
		std::string						_ipAddr;
		int								_flag;
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
		size_t									getMaxBodySize(void) const;
		const std::vector<unsigned short>&		getPort(void) const;
		const std::string&						getServerName(void) const;
		const std::string&						getRoot(void) const;
		const std::string&						getIndex(void) const;
		const std::string&						getErrorPath(void) const;
		const std::string&						getIpAddr(void)	const;
		const std::map<std::string, Location>&	getLocation(void) const;
		void	setFlag(int nb);

		/* MEMBERS FUNCTIONS */
		void									print(void) const;
		void									checkAttribut(void) const;

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
		class NoDefaultException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "No default location specified.\n";
				}
		};
		class MoreThanOneDefaultException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "More than one default location specified.\n";
				}
		};
};

#endif
