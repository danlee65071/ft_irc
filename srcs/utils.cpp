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
		exit_program("Usage: ./ircserv <port> <password>");
	port = atoi(argv[1]);
	if (port < 1024 || port > 65535)
		exit_program("Wrong port!");
	return port;
}

int		send_reply(const std::string &from, const Client &user, int rpl, const std::string& arg1, const std::string& arg2, \
				 const std::string& arg3, const std::string& arg4, const std::string& arg5, const std::string& arg6, \
				 const std::string& arg7, const std::string& arg8)
{
	std::string	msg = ":" + from + " ";
	std::stringstream	ss;
	ss << rpl;
	msg += ss.str() + " " + user.get_nick() + " ";
	switch (rpl)
	{
		case RPL_USERHOST:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_ISON:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_AWAY:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_UNAWAY:
			msg += ":You are no longer marked as being away\n";
			break;
		case RPL_NOWAWAY:
			msg += ":You have been marked as being away\n";
			break;
		case RPL_WHOISUSER:
			msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
			break;
		case RPL_WHOISSERVER:
			msg += arg1 + " " + arg2 + " :" + arg3 + "\n";
			break;
		case RPL_WHOISOPERATOR:
			msg += arg1 + " :is an IRC operator\n";
			break;
		case RPL_WHOISIDLE:
			msg += arg1 + " " + arg2 + " " + arg3 + " :seconds idle\n";
			break;
		case RPL_ENDOFWHOIS:
			msg += arg1 + " :End of /WHOIS list\n";
			break;
		case RPL_WHOISCHANNELS:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_WHOWASUSER:
			msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
			break;
		case RPL_ENDOFWHOWAS:
			msg += arg1 + " :End of WHOWAS\n";
			break;
		case RPL_LISTSTART:
			msg += "Channel :Users  Name\n";
			break;
		case RPL_LIST:
			msg += arg1 +  " " + arg2 + " :" + arg3 + "\n";
			break;
		case RPL_LISTEND:
			msg += ":End of /LIST\n";
			break;
		case RPL_CHANNELMODEIS:
			msg += arg1 + " +" + arg2 + "\n";
			break;
		case RPL_NOTOPIC:
			msg += arg1 + " :No topic is set\n";
			break;
		case RPL_TOPIC:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_INVITING:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_SUMMONING:
			msg += arg1 + " :Summoning user to IRC\n";
			break;
		case RPL_VERSION:
			msg += arg1 + "." + arg2 + " " + arg3 + " :" + arg4 + "\n";
			break;
		case RPL_WHOREPLY:
			msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
			msg += arg5 + " " + arg6 + " :" + arg7 + " " + arg8 + "\n";
			break;
		case RPL_ENDOFWHO:
			msg += arg1 + " :End of /WHO list\n";
			break;
		case RPL_NAMREPLY:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_ENDOFNAMES:
			msg += arg1 + " :End of /NAMES list\n";
			break;
		case RPL_LINKS:
			msg += arg1 + " " + arg2 + ": " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_ENDOFLINKS:
			msg += arg1 + " :End of /LINKS list\n";
			break;
		case RPL_BANLIST:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_ENDOFBANLIST:
			msg += arg1 + " :End of channel ban list\n";
			break;
		case RPL_INFO:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_ENDOFINFO:
			msg += ":End of /INFO list\n";
			break;
		case RPL_MOTDSTART:
			msg += ":- " + arg1 + " Message of the day - \n";
			break;
		case RPL_MOTD:
			msg += ":- " + arg1 + "\n";
			break;
		case RPL_ENDOFMOTD:
			msg += ":End of /MOTD command\n";
			break;
		case RPL_YOUREOPER:
			msg += ":You are now an IRC operator\n";
			break;
		case RPL_REHASHING:
			msg += arg1 + " :Rehashing\n";
			break;
		case RPL_TIME:
			msg += arg1 + " :" + arg2;
			break;
		case RPL_USERSSTART:
			msg += ":UserID   Terminal  Host\n";
			break;
		case RPL_USERS:
			msg += ":%-8s %-9s %-8s\n";
			break;
		case RPL_ENDOFUSERS:
			msg += ":End of users\n";
			break;
		case RPL_NOUSERS:
			msg += ":Nobody logged in\n";
			break;
		case RPL_TRACELINK:
			msg += "Link " + arg1 + " " + arg2 + " " + arg3 + "\n";
			break;
		case RPL_TRACECONNECTING:
			msg += "Try. " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEHANDSHAKE:
			msg += "H.S. " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEUNKNOWN:
			msg += "???? " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEOPERATOR:
			msg += "Oper " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEUSER:
			msg += "User " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACESERVER:
			msg += "Serv " + arg1 + " " + arg2 + "S " + arg3 + "C ";
			msg += arg4 + " " + arg5 + "@" + arg6 + "\n";
			break;
		case RPL_TRACENEWTYPE:
			msg += arg1 + " 0 " + arg2 + "\n";
			break;
		case RPL_TRACELOG:
			msg += "File " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_STATSLINKINFO:
			msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
			msg += arg5 + " " + arg6 + " " + arg7 + "\n";
			break;
		case RPL_STATSCOMMANDS:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_STATSCLINE:
			msg += "C " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSNLINE:
			msg += "N " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSILINE:
			msg += "I " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSKLINE:
			msg += "K " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSYLINE:
			msg += "Y " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_ENDOFSTATS:
			msg += arg1 + " :End of /STATS report\n";
			break;
		case RPL_STATSLLINE:
			msg += "L " + arg1 + " * " + arg2 + " " + arg3 + "\n";
			break;
		case RPL_STATSUPTIME:
			msg += ":Server Up %d days %d:%02d:%02d\n";
			break;
		case RPL_STATSOLINE:
			msg += "O " + arg1 + " * " + arg2 + "\n";
			break;
		case RPL_STATSHLINE:
			msg += "H " + arg1 + " * " + arg2 + "\n";
			break;
		case RPL_UMODEIS:
			msg += arg1 + "\n";
			break;
		case RPL_LUSERCLIENT:
			msg += ":There are " + arg1 + " users and " + arg2;
			msg += " invisible on " + arg3 + " servers\n";
			break;
		case RPL_LUSEROP:
			msg += arg1 + " :operator(s) online\n";
			break;
		case RPL_LUSERUNKNOWN:
			msg += arg1 + " :unknown connection(s)\n";
			break;
		case RPL_LUSERCHANNELS:
			msg += arg1 + " :channels formed\n";
			break;
		case RPL_LUSERME:
			msg += ":I have " + arg1 + " clients and " + arg2 + " servers\n";
			break;
		case RPL_ADMINME:
			msg += arg1 + " :Administrative info\n";
			break;
		case RPL_ADMINLOC1:
			msg += ":Name     " + arg1 + "\n";
			break;
		case RPL_ADMINLOC2:
			msg += ":Nickname " + arg1 + "\n";
			break;
		case RPL_ADMINEMAIL:
			msg += ":E-Mail   " + arg1 + "\n";
			break;
		default:
			msg += "UNKNOWN REPLY\n";
			break;
	}
	send(user.get_socket(), msg.c_str(), msg.size(), SO_NOSIGPIPE);
	return 0;
}

int		send_error(const Client &user, int err, const std::string &arg1, const std::string &arg2)
{
	std::string server_name = SERVER_NAME;
	std::string	msg = ":" + server_name + " ";
	std::stringstream	ss;
	ss << err;
	msg += ss.str() + " " + server_name;
	switch (err)
	{
		case ERR_NOSUCHNICK:
			msg += " " + arg1 + " :No such nick/channel\n";
			break;
		case ERR_NOSUCHSERVER:
			msg += " " + arg1 + " :No such server\n";
			break;
		case ERR_NOSUCHCHANNEL:
			msg += " " + arg1 + " :No such channel\n";
			break;
		case ERR_CANNOTSENDTOCHAN:
			msg += " " + arg1 + " :Cannot send to channel\n";
			break;
		case ERR_TOOMANYCHANNELS:
			msg += " " + arg1 + " :You have joined too many channels\n";
			break;
		case ERR_WASNOSUCHNICK:
			msg += " " + arg1 + " :There was no such nickname\n";
			break;
		case ERR_TOOMANYTARGETS:
			msg += " " + arg1 + " :Duplicate recipients. No arg1 delivered\n";
			break;
		case ERR_NOORIGIN:
			msg += " :No origin specified\n";
			break;
		case ERR_NORECIPIENT:
			msg += " :No recipient given (" + arg1 + ")\n";
			break;
		case ERR_NOTEXTTOSEND:
			msg += " :No text to send\n";
			break;
		case ERR_NOTOPLEVEL:
			msg += " " + arg1 + " :No toplevel domain specified\n";
			break;
		case ERR_WILDTOPLEVEL:
			msg += " " + arg1 + " :Wildcard in toplevel domain\n";
			break;
		case ERR_UNKNOWNCOMMAND:
			msg += " " + arg1 + " :Unknown command\n";
			break;
		case ERR_NOMOTD:
			msg += " :MOTD File is missing\n";
			break;
		case ERR_NOADMININFO:
			msg += " " + arg1 + " :No administrative info available\n";
			break;
		case ERR_FILEERROR:
			msg += " :File error doing \n" + arg1 + " on " + arg2 + "\n";
			break;
		case ERR_NONICKNAMEGIVEN:
			msg += " :No nickname given\n";
			break;
		case ERR_ERRONEUSNICKNAME:
			msg += " " + arg1 + " :Erroneus nickname\n";
			break;
		case ERR_NICKNAMEINUSE:
			msg += " " + arg1 + " :Nickname is already in use\n";
			break;
		case ERR_NICKCOLLISION:
			msg += " " + arg1 + " :Nickname collision KILL\n";
			break;
		case ERR_USERNOTINCHANNEL:
			msg += " " + arg1 + " " + arg2 + " :They aren't on that channel\n";
			break;
		case ERR_NOTONCHANNEL:
			msg += " " + arg1 + " :You're not on that channel\n";
			break;
		case ERR_USERONCHANNEL:
			msg += " " + arg1 + " " + arg2 + " :is already on channel\n";
			break;
		case ERR_NOLOGIN:
			msg += " " + arg1 + " :User not logged in\n";
			break;
		case ERR_SUMMONDISABLED:
			msg += " :SUMMON has been disabled\n";
			break;
		case ERR_USERSDISABLED:
			msg += " :USERS has been disabled\n";
			break;
		case ERR_NOTREGISTERED:
			msg += " :You have not registered\n";
			break;
		case ERR_NEEDMOREPARAMS:
			msg += " " + arg1 + " :Not enough parameters\n";
			break;
		case ERR_ALREADYREGISTRED:
			msg += " :You may not reregister\n";
			break;
		case ERR_NOPERMFORHOST:
			msg += " :Your host isn't among the privileged\n";
			break;
		case ERR_PASSWDMISMATCH:
			msg += " :Password incorrect\n";
			break;
		case ERR_YOUREBANNEDCREEP:
			msg += " :You are banned from this server\n";
			break;
		case ERR_KEYSET:
			msg += " " + arg1 + " :Channel key already set\n";
			break;
		case ERR_CHANNELISFULL:
			msg += " " + arg1 + " :Cannot join channel (+l)\n";
			break;
		case ERR_UNKNOWNMODE:
			msg += " " + arg1 + " :is unknown mode char to me\n";
			break;
		case ERR_INVITEONLYCHAN:
			msg += " " + arg1 + " :Cannot join channel (+i)\n";
			break;
		case ERR_BANNEDFROMCHAN:
			msg += " " + arg1 + " :Cannot join channel (+b)\n";
			break;
		case ERR_BADCHANNELKEY:
			msg += " " + arg1 + " :Cannot join channel (+k)\n";
			break;
		case ERR_NOPRIVILEGES:
			msg += " :Permission Denied- You're not an IRC operator\n";
			break;
		case ERR_CHANOPRIVSNEEDED:
			msg += " " + arg1 + " :You're not channel operator\n";
			break;
		case ERR_CANTKILLSERVER:
			msg += " :You cant kill a server!\n";
			break;
		case ERR_NOOPERHOST:
			msg += " :No O-lines for your host\n";
			break;
		case ERR_UMODEUNKNOWNFLAG:
			msg += " :Unknown MODE flag\n";
			break;
		case ERR_USERSDONTMATCH:
			msg += " :Cant change mode for other users\n";
			break;
		default:
			msg += "UNKNOWN ERROR\n";
			break;
	}
	send(user.get_socket(), msg.c_str(), msg.size(), SO_NOSIGPIPE);
	return (-1);
}

bool	is_equal_to_regex(std::string mask, std::string str)
{
	const char *rs=0;
	const char *rp;
	const char *s = str.c_str();
	const char *p = mask.c_str();
	while (1)
	{
		if (*p == '*')
		{
			rs = s;
			rp = ++p;
		}
		else if (!*s)
			return (!(*p));
		else if (*s == *p)
		{
			++s;
			++p;
		}
		else if (rs)
		{
			s = ++rs;
			p = rp;
		}
		else
			return false;
	}
}

void	log_message(const Message &msg)
{
	std::cout << std::endl << "prefix = " << msg.get_prefix() << ", command = " << msg.get_command();
	std::cout << ", paramsCount = " << msg.get_params().size() << std::endl;
	const std::vector<std::string>	params = msg.get_params();
	size_t	paramsSize = params.size();
	for (size_t i = 0; i < paramsSize; i++)
	{
		if (i == 0)
			std::cout << "Params list: \"" << params[i] << "\"";
		else
			std::cout << ", \"" << params[i] << "\"";
		if (i == (paramsSize - 1))
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

bool	is_valid_nick(const std::string &nick)
{
	if (nick.length() > 9)
		return (false);
	std::string	special = "-[]\\`^{}";
	for (size_t i = 0; i < nick.size(); i++)
	{
		if ((nick[i] >= 'a' && nick[i] <= 'z')
			|| (nick[i] >= 'A' && nick[i] <= 'Z')
			|| (nick[i] >= '0' && nick[i] <= '9')
			|| (special.find(nick[i]) != std::string::npos))
			continue ;
		else
			return (false);
	}
	return (true);
}
