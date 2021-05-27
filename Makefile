NAME			= webserv

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

INCLUDES		= -I includes/Config -I includes/Utils -I includes/HTTP

CC					= clang++
CFLAGS 			= -Wall -Wextra -Werror -std=c++98 -pthread
RM					= rm -f

all:				$(NAME)

$(NAME):		$(OBJS)
						$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

%.o: %.cpp
						${CC} ${CFLAGS} $(INCLUDES) -o $@ -c $<

clean:
						${RM} ${OBJS}

fclean:			clean
						${RM} ${NAME}

re:					fclean all

.PHONY: 		all fclean clean re