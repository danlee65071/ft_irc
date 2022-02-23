#ifndef FT_IRC_MESSAGE_HPP
#define FT_IRC_MESSAGE_HPP

# include "utils.hpp"

class Message
{
private:
	std::string						prefix;
	std::string						command;
	std::vector<std::string>		parameters;

	Message();
	Message(const Message& copy);
	Message	&operator=(const Message& other);

	std::queue<std::string> fill(const std::string &s, char sep);
public:
	Message(const std::string &str);
	virtual ~Message();
	const std::string				&get_prefix() const;
	const std::string				&get_command() const;
	const std::vector<std::string>	&get_params() const;
};

#endif
