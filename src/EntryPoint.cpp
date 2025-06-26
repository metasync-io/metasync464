#include "epch.h"
#include "Server.h"

int main()
{
	Skeleton::Server server("0.0.0.0", 43594);
	server.Run();
}