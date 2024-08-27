// On ne fait pas de Header pour le moment :)

#include "Location.hpp"
#include "ServerConf.hpp"
#include "Servers.hpp"

#include "../include/SocketManager.hpp"

int main (int ac, char **av)
{
	if (ac > 1){
		Servers	serv(av[1]);
		ServerConf servOne = serv.getServConf(1);
		SocketManager socket(servOne.getPort());
	
	
	
	
		serv.printConfigs();

		socket.setHost(servOne.getIpAddr());
	
	
		if (!socket.createSocket() || !socket.bindSocket() || !socket.startListening())
		{
			return (-1);
		}
	
		socket.start();
		return (0);
	}
	else if (ac == 1)
	{
		std::cout << RED << "Server should start with default values" << RESET << std::endl;
	}
	return (0);
}

/*
int main (int argc, char *argv[]) {

	if (argc != 2){
		std::cout << "Error: Invalid argument" << std::endl;
		return (0);
	}
	Servers	serv(argv[1]);
	
	serv.printConfigs();

	return 0;
}
*/
