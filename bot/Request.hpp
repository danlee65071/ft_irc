#ifndef FT_IRC_REQUEST_HPP
#define FT_IRC_REQUEST_HPP

# include "utils.hpp"

class Request
{
private:
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
	std::string					request;
	std::queue<std::string>		params_queue;

	Request();
	Request(const Request& other);
	Request& operator=(const Request& other);

	void get_params_queue();
	void find_prefix();
	void find_command();
	void find_params();
public:
	Request(const std::string& Request);
	~Request();

	const std::string& get_prefix() const;
	const std::string& get_command() const;
	const std::vector<std::string>& get_params() const;
};


#endif
