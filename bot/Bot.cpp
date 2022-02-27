#include "Bot.hpp"

bool Bot::is_working = true;

Bot::Bot(int Port, const std::string& Password): bot_name(BOT_NAME), port(Port), server_password(Password) , bot_nick(BOT_NAME)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	addr_len = sizeof(addr);
}

Bot::~Bot()
{
	close(socket_bot);
}

void Bot::socket_init()
{
	if ((socket_bot = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		exit_program("Socket initialization error!");
}

void Bot::socket_connect()
{
	if (connect(socket_bot, reinterpret_cast<struct sockaddr*>(&addr), addr_len) < 0)
		exit_program("Connect error!");
}

void Bot::send_message(const std::string& message)
{
	if (send(socket_bot, message.c_str(), message.length(), SO_NOSIGPIPE) < 0)
		exit_program("Send message error!");
}

void Bot::recv_socket()
{
	memset(recv_buf, 0, BUFF_SIZE);
	request = "";
	while ((recv_bytes = recv(socket_bot, recv_buf, BUFF_SIZE - 1, 0)) > 0)
	{
		recv_buf[recv_bytes] = '\0';
		request += recv_buf;
	}
}

void Bot::bot_login()
{
	std::string login[3];

	login[0] = "PASS " + server_password + "\n";
	login[1] = "USER " + bot_name + " 127.0.0.1" + " ft_irc" + " " + bot_nick + "\n";
	login[2] = "NICK " + bot_nick + "\n";
	for (int i = 0; i < 3; ++i)
		send_message(login[i]);
}

void Bot::sig_process(int sig_code)
{
	(void)sig_code;
	is_working = false;
}

std::string get_sender(const std::string& prefix)
{
	char sender[512] = {0};
	sscanf(prefix.c_str(), "%[^!]", sender);

	return std::string(sender);
}

void Bot::process_request()
{
	static std::string buf = "";

	while (!request_queue.empty())
		request_queue.pop();
	get_request_queue(buf + request);
	while (request_queue.size() > 0)
	{
		if (request_queue.front().find('\n') != std::string::npos)
		{
			Request req(request_queue.front());
			if (req.get_command() == "PRIVMSG")
			{
				content = "";
				for (std::vector<std::string>::const_iterator it = req.get_params().begin() + 1; it != req.get_params().end(); ++it)
					content += upper_str(*it);
				sender = ::get_sender(req.get_prefix());
				send_message("PRIVMSG " + sender + " :" + content + "\n");
				std::cout << "PRIVMSG " + sender + " :" + content + "\n";
			}
			else if (req.get_command() == "PING")
				send_message("PONG :" + req.get_params()[1]);
		}
		else
			buf = request_queue.front();
		request_queue.pop();
	}
}

std::string Bot::upper_str(std::string str)
{
	std::transform(str.begin(), str.end(),str.begin(), ::toupper);
	return str;
}

void Bot::get_request_queue(const std::string& req)
{
	std::string::const_iterator it = req.begin();
	std::string::const_iterator slash_n_it;

	while (it != req.end())
	{
		while (it != req.end() && *it == '\n')
			++it;
		slash_n_it = std::find(it, req.end(), '\n');
		if (it != req.end())
		{
			request_queue.push(std::string(it, slash_n_it + 1));
			std::cout << std::string(it, slash_n_it + 1) << std::endl;
			it = slash_n_it;
		}
	}
}

void Bot::bot_start()
{
	socket_init();
	socket_connect();
	fcntl(socket_bot, F_SETFL, O_NONBLOCK);
	bot_login();
	signal(SIGINT, sig_process);
	while(is_working)
	{
		recv_socket();
		process_request();
	}
}
