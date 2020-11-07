# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: abobas <abobas@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/04 14:02:54 by abobas        #+#    #+#                  #
#    Updated: 2020/11/07 12:22:50 by abobas        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

SRC = 			main.cpp \
				Server.cpp \
				Socket.cpp \
				logger/Log.cpp \
				config/Json.cpp \
				incoming/Receiver.cpp \
				incoming/Evaluator.cpp \
				incoming/Parser.cpp \
				incoming/Matcher.cpp \
				incoming/Validator.cpp \
				processor/Processor.cpp \
				processor/Upload.cpp \
				processor/Cgi.cpp \
				outgoing/Responder.cpp \
				outgoing/Resolver.cpp \
				outgoing/Directory.cpp

SRC_DIR	=		./src/

SRC :=			$(SRC:%=$(SRC_DIR)%)
			
INCLUDE :=		$(INCLUDE:%=$(INCLUDE_DIR)%)

NAME = 			webserv

FLAGS =			-Wall -Werror -Wextra -std=c++17

all: 			$(NAME)

$(NAME):		$(SRC)
				$(CXX) $(FLAGS) $(SRC) -o $(NAME)

run:
				sudo ./webserv --config config/config.json

tester:
				sudo ./webserv --config test_bin/test.json

clean:
				rm -rf $(NAME)

re:				clean all
