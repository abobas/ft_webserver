# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: abobas <abobas@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/07/04 14:02:54 by abobas        #+#    #+#                  #
#    Updated: 2020/07/07 20:56:15 by abobas        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

SRC = 			main.cpp \
				Server.cpp

SRC_DIR	=		./src/

SRC :=			$(SRC:%=$(SRC_DIR)%)

INCLUDE =		Server.hpp

INCLUDE_DIR =	./src/includes/
			
INCLUDE :=		$(INCLUDE:%=$(INCLUDE_DIR)%)

NAME = 			webserv

FLAGS =			-Wall -Werror -Wextra -std=c++98 -pedantic

CC =			clang++

all: 			$(NAME)

$(NAME):		$(SRC) $(INCLUDE)
				$(CC) $(FLAGS) $(SRC) -o $(NAME)

clean:
				rm -rf *.o

fclean:			clean
				rm -rf $(NAME)

re:				fclean all
