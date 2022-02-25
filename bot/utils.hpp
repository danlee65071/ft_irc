#ifndef FT_IRC_UTILS_HPP
#define FT_IRC_UTILS_HPP

# include <iostream>
# include <map>
# include <cerrno>
# include <vector>
# include <set>
# include <queue>
# include <unistd.h>
# include <cstdlib>
# include <algorithm>
# include <sys/socket.h>
# include <fcntl.h>
# include <ctime>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/poll.h>
# include <sys/event.h>
# include <csignal>
# include <sstream>
# include <fstream>
# include "Request.hpp"


# define RED		"\033[31m"
# define GREEN	"\033[32m"
# define YELLOW	"\033[33m"
# define RESET	"\033[0m"

# define BOT_NAME "irc_bot"
# define SERVER_NAME "ft_irc"
# define BUFF_SIZE 1024

void	exit_program(const std::string &error_msg);
int		check_input_params(int argc, char** argv);

#endif
