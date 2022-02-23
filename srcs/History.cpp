#include "History.hpp"

History::History() {}

History::~History()
{
	while (history_list.size() > 0)
	{
		delete history_list[0];
		history_list.erase(history_list.begin());
	}
}

void	History::add_user(const Client &member)
{
	this->history_list.push_back(new UserInfo(member));
}

const std::vector<const UserInfo *>	History::get_history(const std::string &nickname) const
{
	std::vector<const UserInfo *> filtered_history;

	for (size_t i = 0; i < history_list.size(); ++i)
	{
		if (history_list[i]->getNickname() == nickname)
			filtered_history.push_back(history_list[i]);
	}
	return filtered_history;
}
