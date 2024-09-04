
#include "Parsing/Servers.hpp"
#include "Parsing/ServerConf.hpp"
#include "Sockets/SocketManager.hpp"

int main (int ac, char **av)
{
	size_t				npos;
	Servers				serv;
	std::string			config_file;

	
	if (ac > 2) {
		std::cerr << "Error: " << RED << "Too many arguments.\n" << RESET;
		return (1);
	}
	else if (ac < 2) {
		std::cout << ORANGE << "No config file provided, starting with default file.\n" << RESET;
		config_file = "configs/default_file.conf";
	}
	else
		config_file = av[1];
	npos = config_file.find_last_of('.');
	if (npos == std::string::npos || config_file.substr(npos + 1) != "conf") {
		std::cerr << "Error: " << RED << "Incorrect extension.\n" << RESET;
		return (2);
	}
	try {
		serv = Servers(config_file);
		SocketManager socket(serv.getServConf(1).getPort());

		serv.printConfigs();
		socket.setHost(serv.getServConf(1).getIpAddr());
		if (!socket.createSocket() || !socket.bindSocket() || !socket.startListening())
			return (-1);
		socket.start();
	}
	catch (std::exception &e){
		std::cout << e.what() << std::endl;
	}
	return (0);
}
