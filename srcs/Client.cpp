#include "Client.hpp"

Client::Client(int Socket_fd, const std::string &Host_ip): socket_fd(Socket_fd), host_ip(Host_ip),
	registration_time(time(NULL)), flags(RECEIVENOTICE)
{}

Client::~Client()
{}

const int& Client::get_socket() const
{
	return socket_fd;
}

const std::string& Client::get_username() const
{
	return username;
}

const std::string& Client::get_host() const
{
	return host_ip;
}

const std::string& Client::get_nick() const
{
	return nick;
}

const std::string& Client::get_password() const
{
	return password;
}

const time_t& Client::get_registration_time() const
{
	return registration_time;
}

const time_t& Client::get_time_last_mes() const
{
	return time_last_mes;
}

const time_t& Client::get_time_ping() const
{
	return time_ping;
}

const std::queue<std::string>& Client::get_messages() const
{
	return messages;
}

const unsigned char& Client::get_flags() const
{
	return flags;
}

const std::string& Client::get_exit_msg() const
{
	return exit_msg;
}

const std::vector<const Chat*>& Client::get_chats() const
{
	return chats;
}

void Client::set_username(const std::string &Username)
{
	username = Username;
}

void Client::set_nick(const std::string &Nick)
{
	nick = Nick;
}

void Client::set_password(const std::string &Password)
{
	password = Password;
}

void Client::set_exit_msg(const std::string &Message)
{
	exit_msg = Message;
}

void Client::set_flag(const unsigned char &flag)
{
	flags |= flag;
	if (flag == BREAKCONNECTION && exit_msg.size() == 0)
		exit_msg = "Client exited";
}

void Client::send_message(const std::string &message) const
{
	if (message.size() > 0)
		send(socket_fd, message.c_str(), message.size(), SO_NOSIGPIPE);
}

void Client::fill_messages(const std::string &str)
{
	typedef std::string::const_iterator const_iterator;
	for (const_iterator it = str.begin(); it != str.end(); ++it)
	{
		for (; it != str.end() && *it == '\n'; ++it)
			;
		const_iterator separator = std::find(it, str.end(), '\n');
		if (it != str.end())
		{
			messages.push(std::string(it, separator + 1));
			it = separator;
		}
	}
}

int Client::read_message()
{
	std::string text;

	if (messages.size() > 0)
		text = messages.front();
	while((recv_bytes = recv(socket_fd, buf, BUFF_SIZE - 1, 0)) > 0)
	{
		buf[recv_bytes] = '\0';
		text += buf;
		buf[0] = 0;
		if (text.find('\n') != std::string::npos)
			break;
	}
	if (text.length() > TEXT_LEN)
		text = text.substr(0, TEXT_LEN - 2) + "\r\n";
	if (recv_bytes == 0)
		return (-1);
	while (text.find("\r\n") != std::string::npos)
		text.replace(text.find("\r\n"), 2, "\n");
	if (text.size() > 1)
		fill_messages(text);
	return 0;
}

void Client::set_realname(const std::string &Realname)
{
	real_name = Realname;
}

const std::string& Client::get_realname() const
{
	return real_name;
}

std::string Client::get_prefix() const
{
	return std::string(nick + "!" + username + "@" + real_name);
}

void Client::delete_message()
{
	if (messages.size() > 0)
		messages.pop();
}

void Client::delete_flag(unsigned char flag)
{
	flags &= ~flag;
}

void Client::update_time_last_mes()
{
	time_last_mes = time(0);
}

void Client::update_time_ping()
{
	time_ping = time(0);
}

void Client::add_chat(const Chat &chat)
{
	chats.push_back(&chat);
}

bool Client::is_in_chat(const std::string &chat) const
{
	for (size_t i = 0; i < chats.size(); i++)
		if (chats[i]->get_name() == chat)
			return true;
	return false;
}

void	Client::delete_chat(const std::string &chat)
{
	iterator begin = chats.begin();
	iterator end = chats.end();
	for (; begin != end; ++begin)
		if ((*begin)->get_name() == chat)
			break ;
	chats.erase(begin);
}
