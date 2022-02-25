NAME = ircserv

DIR_SRCS = srcs
DIR_HEADERS = includes
DIR_OBJS = objs

SRCS = main.cpp Chat.cpp Client.cpp Hash.cpp History.cpp Message.cpp Server.cpp UserInfo.cpp utils.cpp

PATH_SRCS = $(addprefix $(DIR_SRCS)/, $(SRCS))

HEADERS = Chat.hpp Client.hpp Hash.hpp History.hpp Message.hpp Server.hpp UserInfo.hpp utils.hpp

PATH_HEADERS = $(addprefix $(DIR_HEADERS)/, $(HEADERS))

OBJS = $(patsubst %.cpp, %.o, $(SRCS))
PATH_OBJS = $(addprefix $(DIR_OBJS)/, $(OBJS))

CC = clang++
RM = rm -rf
FLAGS = -Wall -Wextra -Werror -std=c++98 -g

GREEN = \033[0;32m
RESET = \033[0m

all: $(NAME)

$(NAME): mkdir_obj write_logo $(PATH_OBJS)
	@echo "$(GREEN)\nObjects were created $(RESET)"
	@$(CC) $(FLAGS) $(PATH_OBJS) -o $@
	@echo "$(GREEN)\nft_irc was compiled $(RESET)"

$(DIR_OBJS)/%.o: $(DIR_SRCS)/%.cpp $(PATH_HEADERS) Makefile
	@$(CC) $(FLAGS) -I $(DIR_HEADERS) -c $< -o $@
	@echo "$(GREEN).$(RESET)\c"

mkdir_obj:
	@mkdir -p $(DIR_OBJS)

write_logo:
	@echo "$(GREEN)\n\
	████████████████████████████████████████████████████████████████████████████\n\
	█░░░░░░░░░░░░░░█░░░░░░░░░░░░░░█░░░░░░░░░░█░░░░░░░░░░░░░░░░███░░░░░░░░░░░░░░█\n\
	█░░▄▀▄▀▄▀▄▀▄▀░░█░░▄▀▄▀▄▀▄▀▄▀░░█░░▄▀▄▀▄▀░░█░░▄▀▄▀▄▀▄▀▄▀▄▀░░███░░▄▀▄▀▄▀▄▀▄▀░░█\n\
	█░░▄▀░░░░░░░░░░█░░░░░░▄▀░░░░░░█░░░░▄▀░░░░█░░▄▀░░░░░░░░▄▀░░███░░▄▀░░░░░░░░░░█\n\
	█░░▄▀░░█████████████░░▄▀░░███████░░▄▀░░███░░▄▀░░████░░▄▀░░███░░▄▀░░█████████\n\
	█░░▄▀░░░░░░░░░░█████░░▄▀░░███████░░▄▀░░███░░▄▀░░░░░░░░▄▀░░███░░▄▀░░█████████\n\
	█░░▄▀▄▀▄▀▄▀▄▀░░█████░░▄▀░░███████░░▄▀░░███░░▄▀▄▀▄▀▄▀▄▀▄▀░░███░░▄▀░░█████████\n\
	█░░▄▀░░░░░░░░░░█████░░▄▀░░███████░░▄▀░░███░░▄▀░░░░░░▄▀░░░░███░░▄▀░░█████████\n\
	█░░▄▀░░█████████████░░▄▀░░███████░░▄▀░░███░░▄▀░░██░░▄▀░░█████░░▄▀░░█████████\n\
	█░░▄▀░░█████████████░░▄▀░░█████░░░░▄▀░░░░█░░▄▀░░██░░▄▀░░░░░░█░░▄▀░░░░░░░░░░█\n\
	█░░▄▀░░█████████████░░▄▀░░█████░░▄▀▄▀▄▀░░█░░▄▀░░██░░▄▀▄▀▄▀░░█░░▄▀▄▀▄▀▄▀▄▀░░█\n\
	█░░░░░░█████████████░░░░░░█████░░░░░░░░░░█░░░░░░██░░░░░░░░░░█░░░░░░░░░░░░░░█\n\
	████████████████████████████████████████████████████████████████████████████\n\
	$(RESET)"

clean:
	@$(RM) $(DIR_OBJS)
	@echo "$(GREEN)clean instruction was executed$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)fclean instruction was executed$(RESET)"

re: fclean all

.PHONY: all logo clean fclean re mkdir_obj write_logo