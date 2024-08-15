// On ne fait pas de Header pour le moment :)

#include "../include/SocketManager.hpp"

int main (int ac, char **av)
{
	SocketManager server;
	
	if (ac > 1)
	{
		std::string configFile = av[1];
	}
	else
	{
		server.setPort(8080);
		server.setHost("0.0.0.0");
	}

	if (!server.createSocket() || !server.bindSocket() || !server.startListening())
	{
		return (-1);
	}
	server.start();

	return (0);
}
