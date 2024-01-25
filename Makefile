SRCS = main.cpp \
    Server.cpp \
    User.cpp \
	Channel.cpp \
    parsing.cpp \
	File.cpp \
	Bot.cpp \
	ircsavvy.cpp \
    commands/registration.cpp \
	commands/commands.cpp \
	commands/ping.cpp \
	commands/join.cpp \
	commands/privmsg.cpp \
	commands/notice.cpp \
	commands/kick.cpp \
	commands/invite.cpp \
	commands/quit.cpp \
	commands/part.cpp \
	commands/topic.cpp \
	commands/mode.cpp \
	commands/oper.cpp \
	commands/send.cpp \
	commands/get.cpp \


OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

NAME = ircserv
CXX = c++
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
HEADER = Server.hpp \
    	 User.hpp \
		 Channel.hpp \
		 File.hpp \
		 Bot.hpp \
		 macro.hpp \

.PHONY: all clean fclean re

all: $(OBJDIR) ${NAME}

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp ${HEADER} | $(OBJDIR)
	@mkdir -p $(dir $@)
	${CXX} ${CFLAGS} -c $< -o $@

${NAME}: $(OBJS)
	${CXX} ${CFLAGS} -o ${NAME} $(OBJS)

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}
	${RM} -r $(OBJDIR)

re: fclean all

