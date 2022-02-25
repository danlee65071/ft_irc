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
	init_cmds();
	parse_motd();
}

Server::~Server()
{
	for (size_t i = 0; i < members.size(); ++i)
	{
		close(members[i]->get_socket());
		delete members[i];
	}
	for (std::map<std::string, Chat *>::const_iterator it = chats.begin(); it != chats.end(); ++it)
		delete (*it).second;
	close(socket_fd);
}

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
	fcntl(socket_fd, F_SETFL, O_NONBLOCK);
}

void Server::accept_socket()
{
	if ((client_fd = accept(socket_fd, reinterpret_cast<struct sockaddr*>(&addr), &addr_len)) >= 0)
	{
		host_ip = inet_ntoa(addr.sin_addr);
		struct pollfd poll_fd;
		poll_fd.fd = client_fd;
		poll_fd.events = POLLIN;
		poll_fd.revents = 0;
		client_fds.push_back(poll_fd);
		members.push_back(new Client(client_fd,SERVER_NAME));
	}
}

void Server::poll_socket()
{
	int res = poll(client_fds.data(), client_fds.size(), timeout);
	if (res != 0)
	{
		for (size_t i = 0; i < client_fds.size(); ++i)
		{
			if (client_fds[i].revents & POLLIN)
			{
				if ((members[i]->read_message() == -1) || (handle_mes(*(members[i])) == -1))
					members[i]->set_flag(BREAKCONNECTION);
			}
			client_fds[i].revents = 0;
		}
	}
}

int Server::handle_mes(Client& member)
{
	while (member.get_messages().size() > 0 && member.get_messages().front()[member.get_messages().front().size() - 1] == '\n')
	{
		Message	msg(member.get_messages().front());
		member.delete_message();
		log_message(msg);
		if (!(member.get_flags() & REGISTERED) && msg.get_command() != "QUIT" && msg.get_command() != "PASS" \
			&& msg.get_command() != "USER" && msg.get_command() != "NICK")
			send_error(member, ERR_NOTREGISTERED);
		else
		{
			try
			{
				int ret = (this->*(irc_cmds.at(msg.get_command())))(msg, member);
				if (ret == -1)
					return (-1);
			}
			catch(const std::exception& e)
			{
				send_error(member, ERR_UNKNOWNCOMMAND, msg.get_command());
			}
		}
	}
	member.update_time_last_mes();
	return (0);
}

int		Server::pass_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		send_error(member, ERR_NEEDMOREPARAMS, msg.get_command());
	else if (member.get_flags() & REGISTERED)
		send_error(member, ERR_ALREADYREGISTRED);
	else
		member.set_password(msg.get_params()[0]);
	return 0;
}

int		Server::nick_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		send_error(member, ERR_NEEDMOREPARAMS, msg.get_command());
	else if (!is_valid_nick(msg.get_params()[0]) || msg.get_params()[0] == SERVER_NAME)
		send_error(member, ERR_ERRONEUSNICKNAME, msg.get_params()[0]);
	else if (is_exist_member(msg.get_params()[0]))
		send_error(member, ERR_NICKNAMEINUSE, msg.get_params()[0]);
	else
	{
		if (member.get_flags() & REGISTERED)
		{
			notify(member, ": " + msg.get_command() + " " + msg.get_params()[0] + "\n");
			history.add_user(member);
		}
		member.set_nick(msg.get_params()[0]);
	}
	return connect_member(member);
}

int Server::user_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 4)
		send_error(member, ERR_NEEDMOREPARAMS, msg.get_command());
	else if (member.get_flags() & REGISTERED)
		send_error(member, ERR_ALREADYREGISTRED);
	else
		member.set_username(msg.get_params()[0]);
	return connect_member(member);
}

int Server::oper_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 2)
		send_error(member, ERR_NEEDMOREPARAMS, msg.get_command());
	else if (irc_cmds.size() == 0)
		send_error(member, ERR_NOOPERHOST);
	else
	{
		try
		{
			std::string	pass = operators.at(msg.get_params()[0]);
			if (hash::hash(msg.get_params()[1]) == pass)
			{
				member.set_flag(IRCOPERATOR);
				return send_reply(SERVER_NAME, member, RPL_YOUREOPER);
			}
		}
		catch(const std::exception& e)
		{}
		send_error(member, ERR_PASSWDMISMATCH);
	}
	return 0;
}

int		Server::quit_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 0)
		member.set_exit_msg(msg.get_params()[0]);
	history.add_user(member);
	return (-1);
}

int 	Server::privmsg_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NORECIPIENT, msg.get_command()));
	if (msg.get_params().size() == 1)
		return (send_error(member, ERR_NOTEXTTOSEND));

	std::queue<std::string> receivers = fill(msg.get_params()[0], ',');
	std::set<std::string> uniq;

	if (msg.get_command() == "NOTICE" && (receivers.size() > 1 || receivers.front()[0] == '#' || receivers.front()[0] == '&'))
		return (send_error(member, ERR_NOSUCHNICK, msg.get_params()[0]));

	while (receivers.size() > 0)
	{
		if (uniq.find(receivers.front()) != uniq.end())
			return (send_error(member, ERR_TOOMANYTARGETS, receivers.front()));
		if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
		{
			if (!is_exist_chat(receivers.front()))
				return (send_error(member, ERR_NOSUCHNICK, receivers.front()));
			if (!chats[receivers.front()]->is_exist_member(member.get_nick()))
				return (send_error(member, ERR_CANNOTSENDTOCHAN, receivers.front()));
		}
		else if (!is_exist_member(receivers.front()))
			return (send_error(member, ERR_NOSUCHNICK, msg.get_params()[0]));
		uniq.insert(receivers.front());
		receivers.pop();
	}
	for (std::set<std::string>::iterator it = uniq.begin(); it != uniq.end(); ++it)
	{
		if ((*it)[0] == '#' || (*it)[0] == '&')
		{
			Chat *receiver_chat = chats[*it];
			if (receiver_chat->get_flags() & MODERATED && (!receiver_chat->is_admin(member) && !receiver_chat->is_speaker(member)))
				send_error(member, ERR_CANNOTSENDTOCHAN, *it);
			else
				receiver_chat->send_message(msg.get_command() + " " + *it + " :" + msg.get_params()[1] + "\n", member);
		}
		else
		{
			if (msg.get_command() == "PRIVMSG" && (get_member(*it)->get_flags() & AWAY))
				send_reply(SERVER_NAME, member, RPL_AWAY, *it, get_member(*it)->get_exit_msg());
			if (msg.get_command() != "NOTICE" || (get_member(*it)->get_flags() & RECEIVENOTICE))
				get_member(*it)->send_message(": " + msg.get_command() + " " + *it + " :" + msg.get_params()[1] + "\n");
		}
	}
	return 0;
}

int Server::away_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
	{
		member.delete_flag(AWAY);
		send_reply(SERVER_NAME, member, RPL_UNAWAY);
	}
	else
	{
		member.set_flag(AWAY);
		member.set_exit_msg(msg.get_params()[0]);
		send_reply(SERVER_NAME, member, RPL_NOWAWAY);
	}
	return 0;
}

int Server::notice_cmd(const Message &msg, Client &member)
{
	privmsg_cmd(msg, member);
	return 0;
}

int Server::who_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NEEDMOREPARAMS, msg.get_command()));

	for (size_t i = 0; i < members.size(); ++i)
	{
		if (is_equal_to_regex(msg.get_params()[0], members[i]->get_nick()) && !(members[i]->get_flags() & INVISIBLE))
		{
			std::string chat_name = "*";
			std::string userStatus = "";
			const std::vector<const Chat*> member_chats = members[i]->get_chats();

			for (int j = member_chats.size() - 1; j >= 0; --j)
			{
				if ((!(member_chats[j]->get_flags() & SECRET) && !(member_chats[j]->get_flags() & PRIVATE)) \
 					|| (member_chats[j]->is_exist_member(member.get_nick())))
				{
					chat_name = member_chats[j]->get_name();
					if (member_chats[j]->is_admin(*(members[i])))
						userStatus = "@";
					else if (member_chats[j]->is_speaker(*(members[i])))
						userStatus = "+";
					break;
				}
			}

			if (msg.get_params().size() == 1  || msg.get_params()[1] != "o" || (msg.get_params()[1] == "o" && (members[i]->get_flags() & IRCOPERATOR)))
				send_reply(SERVER_NAME, member, RPL_WHOREPLY, chat_name, members[i]->get_username(), members[i]->get_host(), \
							SERVER_NAME, members[i]->get_nick(), "H" + userStatus, "0");
		}
	}
	return (send_reply(SERVER_NAME, member, RPL_ENDOFWHO, member.get_nick()));
}

int Server::whois_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NONICKNAMEGIVEN));

	bool is_nick = false;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (is_equal_to_regex(msg.get_params()[0], members[i]->get_nick()) && !(members[i]->get_flags() & IRCOPERATOR))
		{
			send_reply(SERVER_NAME, member, RPL_WHOISUSER, members[i]->get_nick(), \
			members[i]->get_username(), members[i]->get_host());

			const std::vector<const Chat*> member_chats = members[i]->get_chats();
			std::string	chats_list;
			for (size_t j = 0; j < member_chats.size(); ++j)
			{
				if ((!(member_chats[j]->get_flags() & SECRET) && !(member_chats[j]->get_flags() & PRIVATE)) \
					|| (member_chats[j]->is_exist_member(member.get_nick())))
				{
					if (j != 0)
						chats_list += " ";
					if (member_chats[j]->is_admin(*(members[i])))
						chats_list += "@'";
					else if (member_chats[j]->is_speaker(*(members[i])))
						chats_list += "+";
					chats_list += member_chats[j]->get_name();
				}
			}
			send_reply(SERVER_NAME, member, RPL_WHOISCHANNELS, members[i]->get_nick(), chats_list);
			send_reply(SERVER_NAME, member, RPL_WHOISSERVER, members[i]->get_nick(), SERVER_NAME, info);
			if (members[i]->get_flags() & AWAY)
				send_reply(SERVER_NAME, member, RPL_AWAY, members[i]->get_nick(), members[i]->get_exit_msg());
			if (members[i]->get_flags() & IRCOPERATOR)
				send_reply(SERVER_NAME, member, RPL_WHOISOPERATOR, members[i]->get_nick());
			std::stringstream	onServer, regTime;
			onServer << (time(0) - members[i]->get_registration_time());
			regTime << members[i]->get_registration_time();
			send_reply(SERVER_NAME, member, RPL_WHOISIDLE, members[i]->get_nick(), onServer.str(), regTime.str());
			is_nick = true;
		}
	}
	if (!is_nick)
		send_error(member, ERR_NOSUCHNICK, msg.get_params()[0]);
	return (send_reply(SERVER_NAME, member, RPL_ENDOFWHOIS, msg.get_params()[0]));
}

int Server::whowas_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NONICKNAMEGIVEN));

	else if (!is_exist_member(msg.get_params()[0]))
	{
		const std::vector<const UserInfo *> history_list = history.get_history(msg.get_params()[0]);
		if (history_list.size() == 0)
			send_error(member, ERR_WASNOSUCHNICK, msg.get_params()[0]);
		else
		{
			int n = 0;
			if (msg.get_params().size() > 1)
				n = atoi(msg.get_params()[1].c_str());
			n = (n == 0) ? history_list.size() : n;

			for (int i = 0; i < n && i < static_cast<int>(history_list.size()); ++i)
			{
				send_reply(SERVER_NAME, member, RPL_WHOWASUSER, history_list[i]->get_nick(), \
					SERVER_NAME, history_list[i]->get_hostname());
				send_reply(SERVER_NAME, member, RPL_WHOISSERVER, history_list[i]->get_nick(), \
					SERVER_NAME, info);
			}
		}
	}
	return (send_reply(SERVER_NAME, member, RPL_ENDOFWHOWAS, msg.get_params()[0]));
}

int Server::mode_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 1)
		send_error(member, ERR_NEEDMOREPARAMS, "MODE");
	else
	{
		if (msg.get_params()[0][0] == '#')
		{
			if (!is_exist_chat(msg.get_params()[0]))
				send_error(member, ERR_NOSUCHCHANNEL, msg.get_params()[0]);
			else if (!chats.at(msg.get_params()[0])->is_admin(member))
				send_error(member, ERR_CHANOPRIVSNEEDED, msg.get_params()[0]);
			else if (!chats.at(msg.get_params()[0])->is_exist_member(member.get_nick()))
				send_error(member, ERR_NOTONCHANNEL, msg.get_params()[0]);
			else if (msg.get_params().size() == 1)
				send_reply(SERVER_NAME, member, RPL_CHANNELMODEIS, msg.get_params()[0], chats.at(msg.get_params()[0])->str_flags());
			else if (handle_flags(msg, member) != -1)
			{
				std::string	flag = msg.get_params()[1];
				std::string	tmp = (flag[1] == 'o' || flag[1] == 'v') ? " " + msg.get_params()[2] : "";
				chats.at(msg.get_params()[0])->send_message("MODE " + msg.get_params()[0] + " " + msg.get_params()[1]  + tmp + "\n", member);
			}
		}
		else
		{
			if (msg.get_params()[0] != member.get_nick())
				send_error(member, ERR_USERSDONTMATCH);
			else
			{
				if (msg.get_params().size() == 1)
				{
					std::string	flags = "+";
					if (member.get_flags() & INVISIBLE)
						flags += "i";
					if (member.get_flags() & RECEIVENOTICE)
						flags += "s";
					if (member.get_flags() & RECEIVEWALLOPS)
						flags += "w";
					if (member.get_flags() & IRCOPERATOR)
						flags += "o";
					send_reply(SERVER_NAME, member, RPL_UMODEIS, flags);
				}
				else if (handle_flags(msg, member) != -1)
					member.send_message(": MODE " + msg.get_params()[0] + " " + msg.get_params()[1] + "\n");
			}
		}
	}
	return 0;
}

int Server::topic_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 1)
		send_error(member, ERR_NEEDMOREPARAMS, "TOPIC");
	else if (!is_exist_chat(msg.get_params()[0]))
		send_error(member, ERR_NOTONCHANNEL, msg.get_params()[0]);
	else
	{
		Chat *tmp = chats.at(msg.get_params()[0]);
		if (!tmp->is_exist_member(member.get_nick()))
			send_error(member, ERR_NOTONCHANNEL, msg.get_params()[0]);
		else if (msg.get_params().size() < 2)
			tmp->display_topic(member);
		else
			tmp->set_topic(member, msg.get_params()[1]);
	}
	return 0;
}

int Server::join_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		send_error(member, ERR_NEEDMOREPARAMS, msg.get_command());
	else
	{
		std::queue<std::string>	chats_queue = fill(msg.get_params()[0], ',');
		std::queue<std::string>	keys;
		if (msg.get_params().size() > 1)
			keys = fill(msg.get_params()[1], ',');
		for (; chats_queue.size() > 0; chats_queue.pop())
		{
			std::string	key = keys.size() ? keys.front() : "";
			if (keys.size() > 0)
				keys.pop();
			if (!is_correct_chat(chats_queue.front()))
				send_error(member, ERR_NOSUCHCHANNEL, chats_queue.front());
			else if (member.get_chats().size() >= 10)
				send_error(member, ERR_TOOMANYCHANNELS, chats_queue.front());
			else if (connect_to_chat(member, chats_queue.front(), key) == 1)
				member.add_chat(*(chats.at(chats_queue.front())));
		}
	}
	return 0;
}

int Server::invite_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 2)
		send_error(member, ERR_NEEDMOREPARAMS, "INVITE");
	else if (!is_exist_member(msg.get_params()[0]))
		send_error(member, ERR_NOSUCHNICK, msg.get_params()[0]);
	else if (!member.is_in_chat(msg.get_params()[1]) || !is_exist_chat(msg.get_params()[1]))
		send_error(member, ERR_NOTONCHANNEL, msg.get_params()[1]);
	else
		invite_to_chat(member, msg.get_params()[0], msg.get_params()[1]);
	return 0;
}

int Server::kick_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 2)
		send_error(member, ERR_NEEDMOREPARAMS, "KICK");
	else if (!is_exist_chat(msg.get_params()[0]))
		send_error(member, ERR_NOSUCHCHANNEL, msg.get_params()[0]);
	else if (!chats.at(msg.get_params()[0])->is_admin(member))
		send_error(member, ERR_CHANOPRIVSNEEDED, msg.get_params()[0]);
	else if (!chats.at(msg.get_params()[0])->is_exist_member(member.get_nick()))
		send_error(member, ERR_NOTONCHANNEL, msg.get_params()[0]);
	else if (!is_exist_member(msg.get_params()[1]))
		send_error(member, ERR_NOSUCHNICK, msg.get_params()[1]);
	else if (!chats.at(msg.get_params()[0])->is_exist_member(msg.get_params()[1]))
		send_error(member, ERR_USERNOTINCHANNEL, msg.get_params()[1], msg.get_params()[0]);
	else
	{
		Chat	*chat = chats.at(msg.get_params()[0]);
		std::string	message = "KICK " + chat->get_name() + " " + msg.get_params()[1] + " :";
		if (msg.get_params().size() > 2)
			message += msg.get_params()[2];
		else
			message += member.get_nick();
		chat->send_message(message + "\n", member);
		chat->disconnect(*(get_member(msg.get_params()[1])));
		get_member(msg.get_params()[1])->delete_chat(msg.get_params()[0]);
	}
	return 0;
}

int Server::part_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() < 1)
		send_error(member, ERR_NEEDMOREPARAMS, "PART");
	else
	{
		std::queue<std::string>	chat_queue = fill(msg.get_params()[0], ',');
		while (chat_queue.size() > 0)
		{
			if (!is_exist_chat(chat_queue.front()))
				send_error(member, ERR_NOSUCHCHANNEL, chat_queue.front());
			else if (!member.is_in_chat(chat_queue.front()))
				send_error(member, ERR_NOTONCHANNEL, chat_queue.front());
			else
			{
				chats.at(chat_queue.front())->send_message("PART " + chat_queue.front() + "\n", member);
				chats.at(chat_queue.front())->disconnect(member);
				member.delete_chat(chat_queue.front());
			}
			chat_queue.pop();
		}
	}
	return 0;
}

int Server::names_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
	{
		std::vector<std::string>	members_without_chat;
		for (size_t i = 0; i < members.size(); i++)
			members_without_chat.push_back(members[i]->get_nick());
		for (std::map<std::string, Chat*>::const_iterator it = chats.begin(); it != chats.end(); ++it)
		{
			if (!((*it).second->get_flags() & SECRET) && !((*it).second->get_flags() & PRIVATE))
			{
				(*it).second->display_members(member);
				for (size_t i = 0; i < members_without_chat.size(); i++)
					if ((*it).second->is_exist_member(members_without_chat[i]))
						members_without_chat.erase(members_without_chat.begin() + i--);
			}
		}
		std::string	names;
		for (size_t i = 0; i < members_without_chat.size(); i++)
		{
			names += members_without_chat[i];
			if (i != (members_without_chat.size() - 1))
				names += " ";
		}
		send_reply(SERVER_NAME, member, RPL_NAMREPLY, "* *", names);
		send_reply(SERVER_NAME, member, RPL_ENDOFNAMES, "*");
	}
	else
	{
		std::queue<std::string>	chat_queue;
		chat_queue = fill(msg.get_params()[0], ',');
		while (chat_queue.size() > 0)
		{
			try
			{
				Chat *tmp = chats.at(chat_queue.front());
				if (!(tmp->get_flags() & SECRET) && !(tmp->get_flags() & PRIVATE))
				{
					tmp->display_members(member);
					send_reply(SERVER_NAME, member, RPL_ENDOFNAMES, tmp->get_name());
				}
			}
			catch(const std::exception& e)
			{}
			chat_queue.pop();
		}
	}
	return 0;
}

int Server::list_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 1 && msg.get_params()[1] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params()[1]));
	std::queue<std::string>	chans;
	std::vector<std::string>	chansToDisplay;
	if (msg.get_params().size() > 0)
	{
		chans = fill(msg.get_params()[0], ',');
		while (chans.size() > 0)
		{
			if (is_exist_chat(chans.front()))
				chansToDisplay.push_back(chans.front());
			chans.pop();
		}
	}
	else
	{
		for (std::map<std::string, Chat*>::const_iterator it = chats.begin(); it != chats.end(); ++it)
			chansToDisplay.push_back((*it).first);
	}
	send_reply(SERVER_NAME, member, RPL_LISTSTART);
	for (size_t i = 0; i < chansToDisplay.size(); ++i)
		chats.at(chansToDisplay[i])->member_chat_info(member);
	send_reply(SERVER_NAME, member, RPL_LISTEND);
	return 0;
}

int Server::wallops_cmd(const Message &msg, Client &member)
{
	if (!(member.get_flags() & IRCOPERATOR))
		return (send_error(member, ERR_NOPRIVILEGES));
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NEEDMOREPARAMS, msg.get_command()));

	const std::vector<Client *> member_list = members;
	for (size_t i = 0; i < member_list.size(); ++i)
		if (member_list[i]->get_flags() & IRCOPERATOR)
			member_list[i]->send_message(": " + msg.get_command() + " :" + msg.get_params()[0] + "\n");
	return 0;
}

int Server::ping_cmd(const Message &msg, Client &member)
{
	std::string name = SERVER_NAME;
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NOORIGIN));
	member.send_message(":" + name + " PONG :" + msg.get_params()[0] + "\n");
	return 0;
}

int Server::pong_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() <= 0 || msg.get_params()[0] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params().size() > 0 ? msg.get_params()[0] : ""));
	member.delete_flag(PINGING);
	return 0;
}

int Server::ison_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NEEDMOREPARAMS, msg.get_command()));

	std::string	nicks;
	for (size_t i = 0; i < msg.get_params().size(); ++i)
	{
		if (is_exist_member(msg.get_params()[i]))
		{
			if (nicks.size() > 0)
				nicks += " ";
			nicks += msg.get_params()[i];
		}
	}
	return (send_reply(SERVER_NAME, member, RPL_ISON, nicks));
}

int Server::userhost_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() == 0)
		return (send_error(member, ERR_NEEDMOREPARAMS, msg.get_command()));

	std::string	text;
	for (size_t i = 0; i < msg.get_params().size() && i < 5; ++i)
	{
		if (is_exist_member(msg.get_params()[i]))
		{
			Client *client = get_member(msg.get_params()[i]);
			if (text.size() > 0)
				text += " ";
			text += msg.get_params()[i];
			if (client->get_flags() & IRCOPERATOR)
				text += "*";
			text += (client->get_flags() & AWAY) ? "=-@" : "=+@";
			text += client->get_host();
		}
	}
	return (send_reply(SERVER_NAME, member, RPL_USERHOST, text));
}

int Server::version_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 0 && msg.get_params()[0] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params()[0]));
	return (send_reply(SERVER_NAME, member, RPL_VERSION, "v1", "1", "SERVER_NAME"));
}

int Server::info_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 0 && msg.get_params()[0] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params()[0]));
	std::queue<std::string>	lines = fill("describe", '\n');
	for (;lines.size() > 0; lines.pop())
		send_reply(SERVER_NAME, member, RPL_INFO, lines.front());
	send_reply(SERVER_NAME, member, RPL_INFO, info);
	return (send_reply(SERVER_NAME, member, RPL_ENDOFINFO));
}

int Server::admin_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 0 && msg.get_params()[0] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params()[0]));
	send_reply(SERVER_NAME, member, RPL_ADMINME, SERVER_NAME);
	send_reply(SERVER_NAME, member, RPL_ADMINLOC1, "admin");
	send_reply(SERVER_NAME, member, RPL_ADMINLOC2, "admin");
	send_reply(SERVER_NAME, member, RPL_ADMINEMAIL, "admin@admin.admin");
	return 0;
}

int Server::time_cmd(const Message &msg, Client &member)
{
	if (msg.get_params().size() > 0 && msg.get_params()[0] != SERVER_NAME)
		return (send_error(member, ERR_NOSUCHSERVER, msg.get_params()[0]));
	time_t tmp = time(0);
	return (send_reply(SERVER_NAME, member, RPL_TIME, SERVER_NAME, ctime(&tmp)));
}

int Server::rehash_cmd(const Message &msg, Client &member)
{
	(void)msg;
	if (!(member.get_flags() & IRCOPERATOR))
		return (send_error(member, ERR_NOPRIVILEGES));
	return (send_reply(SERVER_NAME, member, RPL_REHASHING));
}

int Server::kill_cmd(const Message &msg, Client &member)
{
	if (!(member.get_flags() & IRCOPERATOR))
		return (send_error(member, ERR_NOPRIVILEGES));
	if (msg.get_params().size() < 2)
		return (send_error(member, ERR_NEEDMOREPARAMS));
	std::string username = msg.get_params()[0];
	if (username == SERVER_NAME)
		return (send_error(member, ERR_CANTKILLSERVER));
	if (!is_exist_member(username))
		send_error(member, ERR_NOSUCHNICK, msg.get_params()[0]);
	Client *userToKill = get_member(username);
	userToKill->send_message(msg.get_params()[1] + "\n");
	userToKill->set_flag(BREAKCONNECTION);
	return 0;
}

int Server::restart_cmd(const Message &msg, Client &member)
{
	(void)msg;
	if (!(member.get_flags() & IRCOPERATOR))
		return (send_error(member, ERR_NOPRIVILEGES));
	for (std::vector<Client *>::iterator it = members.begin(); it != members.end(); ++it)
		(*it)->set_flag(BREAKCONNECTION);
	close(socket_fd);
	socket_init();
	bind_socket();
	listen_socket();
	return 0;
}

void Server::sig_process(int sig_code)
{
	(void)sig_code;
	is_working = false;
}

bool Server::is_exist_member(const std::string &nick) const
{
	size_t	usersCount = members.size();
	for (size_t i = 0; i < usersCount; i++)
	{
		if (members[i]->get_nick() == nick)
			return (true);
	}
	return (false);
}

void Server::notify(Client &member, const std::string &msg)
{
	const std::vector<const Chat *> chans = member.get_chats();
	for (size_t i = 0; i < members.size(); i++)
	{
		for (size_t j = 0; j < chans.size(); j++)
		{
			if (chans[j]->is_exist_member(members[i]->get_nick()))
			{
				members[i]->send_message(msg);
				break ;
			}
		}
	}
}

int Server::connect_member(Client &member)
{
	if (member.get_nick().size() > 0 && member.get_username().size() > 0)
	{
		if (password.size() == 0 || member.get_password() == password)
		{
			if (!(member.get_flags() & REGISTERED))
			{
				member.set_flag(REGISTERED);
				send_motd(member);
			}
		}
		else
			return -1;
	}
	return 0;
}

void Server::send_motd(const Client &member) const
{
	if (motd.size() == 0)
		send_error(member, ERR_NOMOTD);
	else
	{
		send_reply(SERVER_NAME, member, RPL_MOTDSTART, SERVER_NAME);
		for (size_t i = 0; i < motd.size(); ++i)
			send_reply(SERVER_NAME, member, RPL_MOTD, motd[i]);
		send_reply(SERVER_NAME, member, RPL_ENDOFMOTD);
	}
}

std::queue<std::string>	Server::fill(const std::string &str, char sep)
{
	std::queue<std::string>	ret;
	std::string::const_iterator	i = str.begin();
	while(i != str.end())
	{
		while (i != str.end() && *i == sep)
			++i;
		std::string::const_iterator	j = std::find(i, str.end(), sep);
		if (i != str.end())
		{
			ret.push(std::string(i, j));
			i = j;
		}
	}
	return ret;
}

bool Server::is_exist_chat(const std::string &name) const
{
	try
	{
		chats.at(name);
		return true;
	}
	catch(const std::exception& e)
	{}
	return false;
}

Client* Server::get_member(const std::string &nick)
{
	Client	*ret;

	for (size_t i = 0; i < members.size(); i++)
		if (members[i]->get_nick() == nick)
			ret = members[i];
	return ret;
}

int Server::handle_flags(const Message &msg, Client &member)
{
	std::string	flag = msg.get_params()[1];

	if (flag == "+i")
		member.set_flag(INVISIBLE);
	else if (flag == "-i")
		member.delete_flag(INVISIBLE);
	else if (flag == "+s")
		member.set_flag(RECEIVENOTICE);
	else if (flag == "-s")
		member.delete_flag(RECEIVENOTICE);
	else if (flag == "+w")
		member.set_flag(RECEIVEWALLOPS);
	else if (flag == "-w")
		member.delete_flag(RECEIVEWALLOPS);
	else if (flag == "+o")
	{}
	else if (flag == "-o")
		member.delete_flag(IRCOPERATOR);
	else
		return send_error(member, ERR_UMODEUNKNOWNFLAG);
	return 0;
}

bool Server::is_correct_chat(const std::string &name)
{
	if (name[0] != '#' && name[0] != '&')
		return false;
	for (size_t i = 1; i < name.size(); i++)
	{
		if (name[i] == ' ' || name[i] == 7 || name[i] == 0 \
			|| name[i] == 13 || name[i] == 10 || name[i] == ',')
			return false;
	}
	return true;
}

int Server::connect_to_chat(const Client& member, const std::string& name, const std::string& password)
{
	try
	{
		Chat *tmp = chats.at(name);
		tmp->connect(member, password);
		return (1);
	}
	catch(const std::exception& e)
	{
		chats[name] = new Chat(name, member, password);
	}
	return (1);
}

void Server::invite_to_chat(const Client &member, const std::string &nick, const std::string &chat_name)
{
	Client	*receiver;
	for (size_t i = 0; i < members.size(); ++i)
		if (members[i]->get_nick() == nick)
			receiver = members[i];
	Chat *chat = chats.at(chat_name);
	if (chat->is_exist_member(nick))
		send_error(member, ERR_USERONCHANNEL, nick, chat_name);
	else
		chat->invite(member, *receiver);
}

void Server::parse_motd()
{
	std::string		line;
	std::ifstream	motdFile("motd/motd");
	if (motdFile.is_open())
	{
		while (getline(motdFile, line))
			motd.push_back(line);
		motdFile.close();
	}
}

void Server::init_cmds()
{
	irc_cmds["PASS"] = &Server::pass_cmd;
	irc_cmds["NICK"] = &Server::nick_cmd;
	irc_cmds["USER"] = &Server::user_cmd;
	irc_cmds["OPER"] = &Server::oper_cmd;
	irc_cmds["QUIT"] = &Server::quit_cmd;
	irc_cmds["PRIVMSG"] = &Server::privmsg_cmd;
	irc_cmds["AWAY"] = &Server::away_cmd;
	irc_cmds["NOTICE"] = &Server::notice_cmd;
	irc_cmds["WHO"] = &Server::who_cmd;
	irc_cmds["WHOIS"] = &Server::whois_cmd;
	irc_cmds["WHOWAS"] = &Server::whowas_cmd;
	irc_cmds["MODE"] = &Server::mode_cmd;
	irc_cmds["TOPIC"] = &Server::topic_cmd;
	irc_cmds["JOIN"] = &Server::join_cmd;
	irc_cmds["INVITE"] = &Server::invite_cmd;
	irc_cmds["KICK"] = &Server::kick_cmd;
	irc_cmds["PART"] = &Server::part_cmd;
	irc_cmds["NAMES"] = &Server::names_cmd;
	irc_cmds["LIST"] = &Server::list_cmd;
	irc_cmds["WALLOPS"] = &Server::wallops_cmd;
	irc_cmds["PING"] = &Server::ping_cmd;
	irc_cmds["PONG"] = &Server::pong_cmd;
	irc_cmds["ISON"] = &Server::ison_cmd;
	irc_cmds["USERHOST"] = &Server::userhost_cmd;
	irc_cmds["VERSION"] = &Server::version_cmd;
	irc_cmds["INFO"] = &Server::info_cmd;
	irc_cmds["ADMIN"] = &Server::admin_cmd;
	irc_cmds["TIME"] = &Server::time_cmd;
	irc_cmds["REHASH"] = &Server::rehash_cmd;
	irc_cmds["RESTART"] = &Server::restart_cmd;
	irc_cmds["KILL"] = &Server::kill_cmd;
}

void Server::connect_members()
{
	std::string name = SERVER_NAME;
	for (size_t i = 0; i < members.size(); i++)
	{
		if (this->members[i]->get_flags() & REGISTERED)
		{
			if (time(0) - this->members[i]->get_time_last_mes() > static_cast<time_t>(120))
			{
				this->members[i]->send_message(":" + name + " PING :" + SERVER_NAME + "\n");
				this->members[i]->update_time_ping();
				this->members[i]->update_time_last_mes();
				this->members[i]->set_flag(PINGING);
			}
			if ((members[i]->get_flags() & PINGING) && time(0) - members[i]->get_time_ping() > static_cast<time_t>(60) )
				members[i]->set_flag(BREAKCONNECTION);
		}
	}
}

void Server::break_connections()
{
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i]->get_flags() & BREAKCONNECTION)
		{
			history.add_user(*(members[i]));
			notify(*(members[i]), ": QUIT :" + members[i]->get_exit_msg() + "\n");
			close(members[i]->get_socket());
			for (std::map<std::string, Chat *>::iterator it = chats.begin(); it != chats.end(); ++it)
				if ((*it).second->is_exist_member(members[i]->get_nick()))
					(*it).second->disconnect(*(members[i]));
			delete members[i];
			members.erase(members.begin() + i);
			client_fds.erase(client_fds.begin() + i);
			--i;
		}
	}
}

void Server::delete_empty_chats()
{
	for (std::map<std::string, Chat *>::const_iterator it = chats.begin(); it != chats.end();)
	{
		if ((*it).second->is_empty())
		{
			delete (*it).second;
			chats.erase((*it).first);
			it = chats.begin();
		}
		else
			++it;
	}
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
		poll_socket();
		connect_members();
		break_connections();
		delete_empty_chats();
	}
}
