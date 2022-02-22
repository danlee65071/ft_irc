#ifndef FT_IRC_CLIENT_HPP
#define FT_IRC_CLIENT_HPP

# include "utils.hpp"

# define BUFF_SIZE 100
# define TEXT_LEN 512

enum Status
{
	REGISTERED = 1,
	INVISIBLE,
	RECEIVENOTICE,
	RECEIVEWALLOPS,
	IRCOPERATOR,
	AWAY,
	PINGING,
	BREAKCONNECTION
};

class Client
{
private:
	int 						socket_fd;
	std::string					password;
	std::string					nick;
	std::string 				username;
	std::string					host_ip;
	std::string					exit_msg;
	time_t						registration_time;
	time_t						time_last_mes;
	time_t						time_ping;
	std::queue<std::string>		messages;
//	std::vector<const Chats*>	chats;
	unsigned char				flags;
	char						buf[BUFF_SIZE];
	int							recv_bytes;

	Client();
	Client(const Client& other);
	Client& operator=(const Client& other);
public:
	Client(int Socket_fd, const std::string& Host_ip);
	~Client();

	const int& get_socket() const;
	const std::string& get_username() const;
	const std::string& get_host() const;
	const std::string& get_nick() const;
	const std::string& get_password() const;
	const time_t& get_registration_time() const;
	const time_t& get_time_last_mes() const;
	const time_t& get_time_ping() const;
//	const std::vector<const Chats*>& get_chats() const;
	const std::queue<std::string>& get_messages() const;
	const unsigned char& get_flags() const;
	const std::string& get_exit_msg() const;

	void set_username(const std::string& Username);
	void set_nick(const std::string& Nick);
	void set_password(const std::string& Password);
	void set_exit_msg(const std::string& Message);
	void set_flag(const unsigned char& flag);

	void send_message(const std::string& message) const;
	int read_message();
private:
	void fill_messages(const std::string& str);
};

#endif
