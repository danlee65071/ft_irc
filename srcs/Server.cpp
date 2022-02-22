#include "Server.hpp"

bool Server::is_working;

Server::Server(int Port, std::string Password): port(Port), password(Password), timeout(1)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	addr_len = sizeof(addr);
	is_working = true;
}

Server::~Server()
{}

void Server::socket_init()
{
	if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		exit_program("Socket initialization error!");
}

void Server::bind_socket()
{
	int	yes = 1;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1
		|| bind(socket_fd, reinterpret_cast<struct sockaddr *>(&addr), addr_len) == -1)
		exit_program("Setsockopt failed!");
}

void Server::listen_socket()
{
	if (listen(socket_fd, BACK_LOG) == -1)
		exit_program("Listen failed!");
}

void Server::accept_socket()
{
	if ((client_fd = accept(socket_fd, reinterpret_cast<struct sockaddr*>(&addr), &addr_len)) == 0)
	{
		std::cerr << YELLOW << "Accepted failed!" << std::endl;
		return ;
	}
	host_ip = inet_ntoa(addr.sin_addr);
	poll_fd.fd = client_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
	client_fds.push_back(poll_fd);
}

void Server::sig_process(int sig_code)
{
	(void)sig_code;
	is_working = false;
}

void Server::start_server()
{
	socket_init();
	bind_socket();
	listen_socket();
	signal(SIGINT, sig_process);
	while(is_working)
	{
		accept_socket();
	}
}
