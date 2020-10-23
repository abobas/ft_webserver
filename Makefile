# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: abobas <abobas@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/04 14:02:54 by abobas        #+#    #+#                  #
#    Updated: 2020/10/23 17:36:33 by abobas        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

SRC = 			main.cpp \
				Data.cpp \
				Server.cpp \
				Socket.cpp \
				Response.cpp \
				File.cpp \
				Directory.cpp \
				Proxy.cpp \
				Cgi.cpp \
				json/Json.cpp \
				http/HttpParser.cpp \
				http/HttpRequest.cpp \
				http/HttpResponse.cpp

SRC_DIR	=		./src/

SRC :=			$(SRC:%=$(SRC_DIR)%)
			
INCLUDE :=		$(INCLUDE:%=$(INCLUDE_DIR)%)

NAME = 			webserv

FLAGS =			-Wall -Werror -Wextra -std=c++17

C =				gcc

all: 			$(NAME)

$(NAME):		$(SRC)
				$(CXX) $(FLAGS) $(SRC) -o $(NAME)

clean:
				rm -rf *.o

fclean:			clean
				rm -rf $(NAME)

re:				fclean all
