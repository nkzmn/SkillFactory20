#include "../include/Server.h"

int main() 
{
	setlocale(LC_ALL, "");

	Server server;

	server.init();
	server.tcpConnect();

	server.serverUpdate();
	server.serverClose();

	return 0;
}
