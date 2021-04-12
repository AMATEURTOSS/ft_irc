.PHONY: all clean fclean re

NAME = server

CC = clang++

# CFLAGS = -Wall -Werror -Wextra -std=c++98 -g

CFLAGS = -std=c++98

HEADER_DIR = . 

SRC_DIR = .

SRC = \
	main.cpp	ft_irc.cpp		Socket.cpp		SocketSet.cpp		utils.cpp			read_conf.cpp	\
	Error.cpp	Member.cpp		Message.cpp		Command.cpp		CommandFactory.cpp		Reply.cpp	\
	Channel.cpp	Server.cpp\
	command/ServerCommand.cpp	\
	command/PassCommand.cpp		\
	command/NickCommand.cpp		\
	command/UserCommand.cpp		\
	command/JoinCommand.cpp		\
	command/PrivmsgCommand.cpp	\
	command/PartCommand.cpp		\
	command/QuitCommand.cpp		\
	command/NamesCommand.cpp	\
	command/SquitCommand.cpp	\
	command/TopicCommand.cpp	\
	command/ModeCommand.cpp		\
	command/ListCommand.cpp		\
	command/AwayCommand.cpp		\
	command/KickCommand.cpp		\
	command/InviteCommand.cpp	\
	command/VersionCommand.cpp	\
	command/IsonCommand.cpp		\
	command/OperCommand.cpp		\
	command/UserhostCommand.cpp	\
	command/TimeCommand.cpp		\
	command/InfoCommand.cpp		\
	command/LinksCommand.cpp	\
	command/NoticeCommand.cpp	\
	command/AdminCommand.cpp	\
	command/RehashCommand.cpp	\
	command/PingCommand.cpp		\
	command/TraceCommand.cpp	\
	command/WhoQuery.cpp		\
	command/WhoisQuery.cpp		\


SRCS = $(addprefix $(SRC_DIR)/, $(SRC))

# OBJS = $(SRCS:%.cpp=%.o)

## 나중에 $(CLFAG) 추가
$(NAME) : $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -I $(HEADER_DIR) -I ./command -o $(NAME) -g

sanitize : $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -I $(HEADER_DIR) -I ./command -o $(NAME) -fsanitize=address
# %.o : %.c
# 		$(CC) -o $@ -c $^ -I $(HEADER_DIR) -I ./command

all : $(NAME)

chatbot :
	$(CC) chatbot.cpp $(CFLAGS) -o chatbot -g

# clean :
# 		rm -f $(OBJS)

fclean :
	rm -f $(NAME)
	rm -rf server.dSYM
	rm -f chatbot
	rm -rf chatbot.dSYM

re : fclean all