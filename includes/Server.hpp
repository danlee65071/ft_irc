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
		std::vector<Client*>					members;
		History									history;
		std::vector<std::string>				motd;
		std::map<std::string, std::string>		operators;
		std::map<std::string, Chat*>			chats;
		std::string								info;

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
		void poll_socket();
	public:
		void start_server();
	private:
		static void sig_process(int sig_code);
		bool is_exist_member(const std::string& nick) const;
		void notify(Client& member, const std::string& msg);
		int connect_member(Client& member);
		std::queue<std::string>	fill(const std::string &str, char sep);
		bool is_exist_chat(const std::string& name) const;
		int handle_flags(const Message &msg, Client& member);
		bool is_correct_chat(const std::string& name);
		int connect_to_chat(const Client& member, const std::string& name, const std::string& password);
		void invite_to_chat(const Client& member, const std::string& nick, const std::string& chat_name);
	public:
		int handle_mes(Client& member);

		int pass_cmd(const Message &msg, Client &member);
		int nick_cmd(const Message &msg, Client &member);
		int user_cmd(const Message &msg, Client &member);
		int oper_cmd(const Message &msg, Client &member);
		int quit_cmd(const Message &msg, Client &member);
		int privmsg_cmd(const Message &msg, Client &member);
		int away_cmd(const Message &msg, Client &member);
		int notice_cmd(const Message &msg, Client &member);
		int who_cmd(const Message &msg, Client &member);
		int whois_cmd(const Message &msg, Client &member);
		int whowas_cmd(const Message &msg, Client &member);
		int mode_cmd(const Message &msg, Client &member);
		int topic_cmd(const Message &msg, Client &member);
		int join_cmd(const Message &msg, Client &member);
		int invite_cmd(const Message &msg, Client &member);
		int kick_cmd(const Message &msg, Client &member);
		int part_cmd(const Message &msg, Client &member);
		int names_cmd(const Message &msg, Client &member);
		int list_cmd(const Message &msg, Client &member);
		int wallops_cmd(const Message &msg, Client &member);
		int ping_cmd(const Message &msg, Client &member);
		int pong_cmd(const Message &msg, Client &member);
		int ison_cmd(const Message &msg, Client &member);
		int userthost_cmd(const Message &msg, Client &member);
		int version_cmd(const Message &msg, Client &member);
		int info_cmd(const Message &msg, Client &member);
		int admin_cmd(const Message &msg, Client &member);
		int time_cmd(const Message &msg, Client &member);
		int rehash_cmd(const Message &msg, Client &member);
		int kill_cmd(const Message &msg, Client &member);
		int restart_cmd(const Message &msg, Client &member);

		void send_motd(const Client& member) const;
		Client* get_member(const std::string& nick);
};

#endif
