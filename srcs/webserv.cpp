
#include "Parsing/Servers.hpp"
#include "Parsing/ServerConf.hpp"
#include "Sockets/SocketManager.hpp"

int	launchServer(const ServerConf& servConf)
{
	SocketManager socket(servConf);

	std::cout << "Launching server " << servConf.getServerName() << "\n";
	socket.setHost(servConf.getIpAddr());
	if (!socket.createSocket() || !socket.bindSocket() || !socket.startListening())
		return (-1);
	socket.start(servConf);
	return (0);
}

int main (int ac, char **av)
{
	size_t				npos;
	Servers				serv;
	std::string			config_file;

	if (ac > 2) {
		std::cerr << RED << "Error: " << RESET << "Too many arguments.\n";
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
		std::cerr << RED << "Error: " << RESET << "Incorrect extension.\n";
		return (2);
	}
	try {
		serv = Servers(config_file);
		serv.printConfigs();
		for (int i = 1; i <= serv.getAmountOfServers(); i++)
			return (launchServer(serv.getServConf(i)));
	}
	catch (const Servers::AlreadyPrintedException &e) {
		(void)e;
	}
	catch (const std::exception &e){
		std::cerr << RED << "Error: " << RESET << e.what();
	}
	return (0);
}
