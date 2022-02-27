# include "Bot.hpp"
# include "utils.hpp"

int main(int argc, char** argv)
{
	int port = check_input_params(argc, argv);
	Bot bot(port, argv[2]);

	bot.bot_start();
	return 0;
}