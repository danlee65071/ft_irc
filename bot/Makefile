NAME = irc_bot

SRCS = main.cpp Bot.cpp utils.cpp Request.cpp
HEADERS = Bot.hpp utils.hpp Request.hpp

DIR_OBJS = obj

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
	@echo "$(GREEN)\nft_irc bot was compiled $(RESET)"

$(DIR_OBJS)/%.o: %.cpp $(PATH_HEADERS) Makefile
	@$(CC) $(FLAGS) -c $< -o $@
	@echo "$(GREEN).$(RESET)\c"

mkdir_obj:
	@mkdir -p $(DIR_OBJS)

write_logo:
	@echo "$(GREEN)\n\
	██╗██████╗░░█████╗░░░░░░░██████╗░░█████╗░████████╗\n\
	██║██╔══██╗██╔══██╗░░░░░░██╔══██╗██╔══██╗╚══██╔══╝\n\
	██║██████╔╝██║░░╚═╝█████╗██████╦╝██║░░██║░░░██║░░░\n\
	██║██╔══██╗██║░░██╗╚════╝██╔══██╗██║░░██║░░░██║░░░\n\
	██║██║░░██║╚█████╔╝░░░░░░██████╦╝╚█████╔╝░░░██║░░░\n\
	╚═╝╚═╝░░╚═╝░╚════╝░░░░░░░╚═════╝░░╚════╝░░░░╚═╝░░░\n\
    $(RESET)"

clean:
	@$(RM) $(DIR_OBJS)
	@echo "$(GREEN)clean instruction was executed$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)fclean instruction was executed$(RESET)"

re: fclean all

.PHONY: all logo clean fclean re mkdir_obj write_logo
