NAME = ircserv

DIR_SRCS = srcs
DIR_HEADERS = includes
DIR_OBJS = objs

SRCS = main.cpp Server.cpp
PATH_SRCS = $(addprefix $(DIR_SRCS)/, $(SRCS))

HEADERS = Server.hpp
PATH_HEADERS = $(addprefix $(DIR_HEADERS)/, $(HEADERS))

OBJS = $(patsubst %.cpp, %.o, $(SRCS))
PATH_OBJS = $(addprefix $(DIR_OBJS)/, $(OBJS))

CC = clang++
RM = rm -rf
FLAGS = -Wall -Wextra -Werror -std=c++98

GREEN = \033[0;32m
RESET = \033[0m

all: $(NAME)

$(NAME): mkdir_obj write_logo $(PATH_OBJS)
	@echo "$(GREEN)\nObjects were created $(RESET)"
	@$(CC) $(FLAGS) -I $(DIR_HEADERS) $(PATH_OBJS) -o $@
	@echo "$(GREEN)\nft_containers was compiled $(RESET)"

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

$(DIR_OBJS)/%.o: $(DIR_SRCS)/%.cpp $(PATH_HEADERS) Makefile
	@$(CC) $(FLAGS) -I $(DIR_HEADERS) -c $< -o $@
	@echo "$(GREEN).$(RESET)\c"

clean:
	@$(RM) $(DIR_OBJS)
	@echo "$(GREEN)clean instruction was executed$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)fclean instruction was executed$(RESET)"

re: fclean all

.PHONY: all logo clean fclean re mkdir_obj write_logo