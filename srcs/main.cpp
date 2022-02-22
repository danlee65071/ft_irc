#include <Server.hpp>

int main(int argc, char** argv)
{
	int port = check_input_params(argc, argv);
	Server server(port, argv[2]);

	server.start_server();
	return 0;
}
