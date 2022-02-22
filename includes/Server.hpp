#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

# include "utils.hpp"

# define BACK_LOG 20

class Server
{
	private:
		typedef int (Server::*irc_methods)();

		int 									port;
		std::string 							password;
		int										timeout;
		static bool								is_working;

		int										socket_fd;
		int										client_fd;
		sockaddr_in								addr;
		socklen_t								addr_len;
		struct pollfd							poll_fd;
		std::vector<struct pollfd>				client_fds;
		char*									host_ip;

		std::map<std::string, irc_methods>		irc_cmds;

		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
	public:
		Server(int Port, std::string Password);
		~Server();
	private:
		void socket_init();
		void bind_socket();
		void listen_socket();
		void accept_socket();
	public:
		void start_server();
	private:
		static void sig_process(int sig_code);
};

#endif
