#include "utils.hpp"

void exit_program(const std::string &error_msg)
{
	std::cerr << RED << error_msg << RESET << std::endl;
	exit(EXIT_FAILURE);
}

int	check_input_params(int argc, char** argv)
{
	int 	port;

	if (argc != 3)
		exit_program("Usage: ./irc_bot <port> <server_password>");
	port = atoi(argv[1]);
	if (port < 1024 || port > 65535)
		exit_program("Wrong port!");
	return port;
}
