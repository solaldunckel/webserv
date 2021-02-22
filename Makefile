NAME			= webserv

SRCS_DIR	= srcs

INCLUDES	= includes

SRCS			= $(wildcard $(SRCS_DIR)/*.cpp)

OBJS			= ${SRCS:.cpp=.o}

CC				= clang++
CFLAGS 		= -Wall -Wextra -Werror -std=c++98
RM				= rm -f

all:				$(NAME)

$(NAME):		$(OBJS)
						$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
						${CC} ${CFLAGS} -o $@ -c $< -I $(INCLUDES)

clean:
						${RM} ${OBJS}

fclean:			clean
						${RM} ${NAME}

re:					fclean all

.PHONY: 		all fclean clean re