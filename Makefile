# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: abobas <abobas@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/04 14:02:54 by abobas        #+#    #+#                  #
#    Updated: 2020/11/04 00:18:37 by abobas        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

SRC = 			main.cpp \
				Server.cpp \
				Socket.cpp \
				logger/Log.cpp \
				config/Json.cpp \
				receive/Receiver.cpp \
				evaluate/Evaluator.cpp \
				evaluate/Parser.cpp \
				evaluate/Matcher.cpp \
				evaluate/Validator.cpp \
				respond/Responder.cpp \
				respond/Directory.cpp \
				respond/Proxy.cpp \
				respond/Cgi.cpp \
				respond/Upload.cpp

SRC_DIR	=		./src/

SRC :=			$(SRC:%=$(SRC_DIR)%)
			
INCLUDE :=		$(INCLUDE:%=$(INCLUDE_DIR)%)

NAME = 			webserv

FLAGS =			-Wall -Werror -Wextra -std=c++17

C =				gcc

all: 			$(NAME)

$(NAME):		$(SRC)
				$(CXX) $(FLAGS) $(SRC) -o $(NAME)

run:
				sudo ./webserv --config config/config.json

tester:
				sudo ./webserv --config test_bin/test.json

clean:
				rm -rf *.o

fclean:			clean
				rm -rf $(NAME)

re:				fclean all
