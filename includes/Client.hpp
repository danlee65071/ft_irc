#ifndef FT_IRC_CLIENT_HPP
#define FT_IRC_CLIENT_HPP

# include "utils.hpp"

# define BUFF_SIZE 100
# define TEXT_LEN 512

class Chat;

#define REGISTERED		0b00000001
#define INVISIBLE		0b00000010
#define RECEIVENOTICE	0b00000100
#define RECEIVEWALLOPS	0b00001000
#define IRCOPERATOR		0b00010000
#define AWAY				0b00100000
#define PINGING			0b01000000
#define BREAKCONNECTION	0b10000000

class Client
{
private:
	typedef std::vector<const Chat *>::iterator iterator;

	int 						socket_fd;
	std::string					password;
	std::string					nick;
	std::string 				username;
	std::string					host_ip;
	std::string					exit_msg;
	std::string					real_name;
	time_t						registration_time;
	time_t						time_last_mes;
	time_t						time_ping;
	std::queue<std::string>		messages;
	std::vector<const Chat*>	chats;
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
	const std::string& get_realname() const;
	const time_t& get_registration_time() const;
	const time_t& get_time_last_mes() const;
	const time_t& get_time_ping() const;
	const std::vector<const Chat*>& get_chats() const;
	const std::queue<std::string>& get_messages() const;
	const unsigned char& get_flags() const;
	const std::string& get_exit_msg() const;
	std::string get_prefix() const;

	void set_username(const std::string& Username);
	void set_nick(const std::string& Nick);
	void set_password(const std::string& Password);
	void set_exit_msg(const std::string& Message);
	void set_flag(const unsigned char& flag);
	void set_realname(const std::string& Realname);
private:
	void fill_messages(const std::string& str);
public:
	void send_message(const std::string& message) const;
	int read_message();
	void delete_message();
	void delete_flag(unsigned char flag);
	void update_time_last_mes();
	void update_time_ping();
	void add_chat(const Chat& chat);
	bool is_in_chat(const std::string& chat) const;
	void delete_chat(const std::string& chat);
};

#endif
