NAME			= webserv

SRCS_DIR	= srcs

FILES			= main.cpp \
						Config.cpp \
						Listen.cpp \
						Parser.cpp \
						Server.cpp

SRCS			= $(addprefix ${SRCS_DIR}/, main.cpp \
							Parser.cpp \
							Location.cpp \
							Listen.cpp \
							Config.cpp \
							Request.cpp \
							Server.cpp)

OBJS			= ${SRCS:.cpp=.o}

CC				= clang++
CFLAGS 		= -Wall -Wextra -Werror -std=c++98
RM				= rm -f

all:				$(NAME)

$(NAME):		$(OBJS)
						$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
						${CC} ${CFLAGS} -o $@ -c $<

clean:
						${RM} ${OBJS}

fclean:			clean
						${RM} ${NAME}

re:					fclean all

.PHONY: 		all fclean clean re