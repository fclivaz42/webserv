// On ne fait pas de Header pour le moment :)

#include "../include/SocketManager.hpp"

int main (int ac, char **av)
{
	(void)ac;
	(void)av;

	SocketManager socketManager;
	if (!socketManager.createSocket() || !socketManager.bindSocket(8080)
			|| !socketManager.startListening())
	{
		return (-1);
	}
	while (true)
	{
		int clientFd = socketManager.acceptConnection();
		if (clientFd != -1)
		{
			close(clientFd);
		}
	}
	return (0);
}
