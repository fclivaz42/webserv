// On ne fait pas de Header pour le moment :)

#include "Location.hpp"
#include "ServerConf.hpp"
#include "Servers.hpp"

int main (int argc, char *argv[]) {

	if (argc != 2){
		std::cout << "Error: Invalid argument" << std::endl;
		return (0);
	}
	Servers	serv(argv[1]);
	
	serv.printConfigs();

	return 0;
}
