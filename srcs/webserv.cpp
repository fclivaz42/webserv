// On ne fait pas de Header pour le moment :)

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
}
