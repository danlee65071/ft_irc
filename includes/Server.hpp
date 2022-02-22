#ifndef FT_IRC_SERVER_HPP
#define FT_IRC_SERVER_HPP

# include <string>

class Server
{
	private:
		int port;
		std::string password;
	public:
		Server(int Port, std::string Password);
		~Server();
};

#endif
