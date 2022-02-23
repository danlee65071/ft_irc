#include "Message.hpp"

Message::Message(const std::string &str)
{
	std::string	strWithoutNL = std::string(str.begin(), str.end() - 1);
	std::queue<std::string>	que = fill(strWithoutNL, ' ');

	if (que.size() > 0 && que.front()[0] == ':')
	{
		prefix = std::string(que.front().begin() + 1, que.front().end());
		que.pop();
	}
	if (que.size() > 0)
	{
		command = que.front();
		que.pop();
	}
	while (que.size() > 0)
	{
		if (que.front()[0] == ':')
		{
			std::string	s(que.front().begin() + 1, que.front().end());
			que.pop();
			while (que.size() > 0)
			{
				s.append(" ");
				s.append(que.front());
				que.pop();
			}
			parameters.push_back(s);
		}
		else
		{
			parameters.push_back(que.front());
			que.pop();
		}
	}
}

Message::~Message()
{}

std::queue<std::string>	Message::fill(const std::string &str, char sep)
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

const std::string	&Message::get_prefix() const
{
	return prefix;
}

const std::string	&Message::get_command() const
{
	return command;
}

const std::vector<std::string>	&Message::get_params() const
{
	return parameters;
}
