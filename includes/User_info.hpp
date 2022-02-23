#ifndef FT_IRC_USER_INFO_HPP
#define FT_IRC_USER_INFO_HPP

# include "utils.hpp"

class UserInfo
{
private:
	UserInfo();
	UserInfo(const UserInfo &copy);
	UserInfo &operator=(const UserInfo &copy);

	std::string							nickname;
	std::string							username;
	std::string							hostname;
	time_t								registration_time;

public:
	UserInfo(const Client &user);
	~UserInfo();

	const std::string				&get_nick() const;
	const std::string				&get_username() const;
	const std::string				&get_hostname() const;
	const time_t					&get_registration_time() const;
};

#endif
