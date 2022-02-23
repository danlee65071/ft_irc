#include "User_info.hpp"

UserInfo::UserInfo(const Client &member) :
		nickname(member.get_nick()), username(member.get_username()), hostname(member.get_host()), registration_time(time(0))
{}

const std::string	&UserInfo::get_nick() const
{
	return nickname;
}

const std::string	&UserInfo::get_username() const
{
	return username;
}

const std::string	&UserInfo::get_hostname() const
{
	return hostname;
}

const time_t	&UserInfo::get_registration_time() const
{
	return registration_time;
}