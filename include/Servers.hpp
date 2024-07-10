#ifndef SERVERS_HPP
# define SERVERS_HPP

#include <iostream>
#include <string>
#include <fstream>


class Servers{
	private:
		

	public: 
		/* CONSTRUCTORS */
		Servers(std::string &conf_file);
		/* COPY CONSTRUCTOR */
		Servers(Servers const &cpy);
		/* DESTRUCTOR */
		~Servers();
		/* SURCHARGED OPERATORS */
		Servers &operator=(Servers const &rhs);
		/* GETTERS */

		/* SETTERS */

		/* MEMBERS FUNCTIONS */

};

#endif
