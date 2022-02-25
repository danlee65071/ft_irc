#ifndef FT_IRC_BOT_HPP
#define FT_IRC_BOT_HPP

# include "iostream"
# include "utils.hpp"

class Bot
{
private:
	int							socket_bot;
	std::string					bot_name;
	sockaddr_in					addr;
	socklen_t					addr_len;
	int							port;
	std::string					server_password;
	std::string					bot_nick;
	char 						recv_buf[BUFF_SIZE];
	std::string					request;
	int							recv_bytes;
	static bool					is_working;
	std::queue<std::string>		request_queue;
	std::string					sender;
	std::string					content;

	Bot();
	Bot(const Bot& other);
	Bot& operator=(const Bot& other);
public:
	Bot(int Port, const std::string& Password);
	~Bot();

private:
	void socket_init();
	void socket_connect();
	void send_message(const std::string& message);
	void recv_socket();
	void bot_login();
	static void sig_process(int sig_code);
	std::string get_sender(const std::string& prefix);
	void process_request();
	void get_request_queue(const std::string& req);
	std::string upper_str(std::string str);
public:
	void bot_start();
};

#endif
