// On ne fait pas de Header pour le moment :)

#include "Location.hpp"
#include "ServerConf.hpp"
#include "Servers.hpp"

#include "../include/SocketManager.hpp"

int main (int ac, char **av)
{
	std::vector<int> ports;
	ports.push_back(8080);
	ports.push_back(3000);
	SocketManager server(ports);
	
	if (ac > 1)
	{
		std::string configFile = av[1];
	}
	else
	{
		server.setHost("0.0.0.0");
		server.setPorts(ports);
	}

	if (!server.createSocket() || !server.bindSocket() || !server.startListening())
	{
		return (-1);
	}
	
	server.start();
	return (0);
#include "Location.hpp"
#include "ServerConf.hpp"
#include "Servers.hpp"
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