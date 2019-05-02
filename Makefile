# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dborysen <dborysen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/02/15 11:35:57 by dborysen          #+#    #+#              #
#    Updated: 2019/04/24 11:08:34 by dborysen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Expert_system

SRCS =  main.cpp ExpertSystem.cpp

OBJECT = $(SRCS:.cpp=.o)

FLAGS = -Wall -Wextra -Werror -std=c++14

CC = clang++

all: $(NAME)

$(NAME) : $(OBJECT)
	@$(CC) $(OBJECT) -o $(NAME)

$(OBJECT) : $(SRCS)
	@$(CC) $(FLAGS) -c $(SRCS)
	@echo "\033[37m[Compilation:\033[0m\033[36m $@\033[0m]"

comp:
	$(CC) -g $(FLAGS) $(SRCS) -o $(NAME)

clean:
	@rm -f $(OBJECT)
	@echo "\033[32m[.o files has been deleted]\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[32m[binary has been deleted]\033[0m"

re: fclean all 