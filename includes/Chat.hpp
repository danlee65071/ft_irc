#ifndef FT_IRC_CHAT_HPP
#define FT_IRC_CHAT_HPP

# include "utils.hpp"

class Client;

#define PRIVATE		0b000001
#define SECRET		0b000010
#define MODERATED	0b000100
#define INVITEONLY	0b001000
#define TOPICSET		0b010000
#define NOMSGOUT		0b100000

class Chat
{
private:
	typedef std::vector<const Client*>::const_iterator const_iterator;

	std::string						name;
	std::string						topic;
	std::vector<const Client*>		admins;
	std::vector<const Client*>		speakers;
	std::vector<const Client*>		members;
	std::vector<const Client*>		invited_members;
	std::vector<std::string>		ban_masks;
	std::string						password;
	unsigned int					client_limit;
	unsigned char					flags;

	Chat();
	Chat(const Chat& other);
	Chat& operator=(const Chat& other);

	void display_chat_info(const Client& member);
public:
	Chat(const std::string& chat_name, const Client& admin, const std::string& chat_password = "");
	virtual ~Chat();

	void display_topic(const Client& member);
	void display_members(const Client& member);
	void send_message(const std::string& message, const Client& sender) const;

	const std::string&	get_name() const;
	const unsigned char&	get_flags() const;

	void set_topic(const Client& member, const std::string& topic_name);
	void set_limit(int limit);
	void set_password(const Client& member, const std::string& pass);
	void set_flag(unsigned char flag);

	bool is_admin(const Client& member) const;
	bool is_speaker(const Client& member) const;
	bool is_empty() const;
	bool is_invited(const Client& member) const;
	bool is_exist_member(const std::string& nickname) const;
	bool is_member_banned(const std::string& mask, const std::string& prefix);

	void connect(const Client &member, const std::string &pass);
	void remove_invited(const Client& member);
	void remove_flag(unsigned char flag);
	void invite(const Client& member, const Client& inviter);
	void add_admin(const Client &member);
	void remove_admin(const Client &member);
	void add_speaker(const Client &member);
	void remove_speaker(const Client &member);
	void add_ban_mask(const std::string& mask);
	void remove_ban_mask(const std::string &mask);
	void disconnect(const Client &member);
	void member_chat_info(const Client &member);
	std::string str_flags() const;
};

#endif
