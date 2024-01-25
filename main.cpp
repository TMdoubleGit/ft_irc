#include "Server.hpp"
#include <iostream>

bool stop_server = false;

static void	signal_handler(int signal)
{
	(void)signal;
	stop_server = true;
}


int main(int argc, char *argv[]) {

	signal(SIGINT, signal_handler);
    if(argc == 3) {
        Server server("TSA-Server", argv[1], argv[2]);
		try {
			server.runServer();
		}
		catch(const std::exception& e) {
			std::cerr << "Caught exception : " << e.what() << std::endl;
		}
    }
    else {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    return 0;
}
