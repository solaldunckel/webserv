NAME			= webserv

NAME_BONUS = webserv_bonus

SRCS_LIST	= \
						main.cpp \
						Config/Config.cpp \
						Config/InputArgs.cpp \
						Config/RequestConfig.cpp \
						Config/ServerConfig.cpp \
						HTTP/CGI.cpp \
						HTTP/Client.cpp \
						HTTP/Request.cpp \
						HTTP/Response.cpp \
						HTTP/Server.cpp \
						Utils/Base64.cpp \
						Utils/File.cpp \
						Utils/get_next_line.cpp \
						Utils/Logger.cpp \
						Utils/MimeTypes.cpp \
						Utils/StatusCode.cpp \
						Utils/StringUtils.cpp \
						Utils/Utils.cpp

SRCS_FOLDER		= srcs

SRCS				= $(addprefix ${SRCS_FOLDER}/, ${SRCS_LIST})

OBJS				= ${SRCS:.cpp=.o}
OBJS_BONUS	= ${SRCS:.cpp=.o_bonus}

INCLUDES		= -I includes/Config -I includes/Utils -I includes/HTTP

CC					= clang++
CFLAGS 			= -Wall -Wextra -Werror -std=c++98 -pthread -g -fsanitize=address
RM					= rm -f

BONUS				= -DBONUS

all:				$(NAME)

$(NAME):		$(OBJS)
						$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(NAME_BONUS):		$(OBJS_BONUS)
									$(CC) $(CFLAGS) $(INCLUDES) $(OBJS_BONUS) -o $(NAME_BONUS)

%.o: %.cpp
						${CC} ${CFLAGS} $(INCLUDES) -o $@ -c $<

%.o_bonus: %.cpp
						${CC} ${CFLAGS} $(BONUS) $(INCLUDES) -o $@ -c $<

bonus:			$(NAME_BONUS)

clean:
						${RM} ${OBJS} ${OBJS_BONUS}

fclean:			clean
						${RM} ${NAME} ${NAME_BONUS}

re:					fclean all

.PHONY: 		all fclean clean re