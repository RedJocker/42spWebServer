#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maurodri <maurodri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/15 17:53:38 by maurodri          #+#    #+#              #
#    Updated: 2025/08/20 22:22:05 by maurodri         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

BASE_DIR := ./src
TEST_DIR := ./test
CONN_DIR := $(addprefix $(BASE_DIR), /conn)
INCLUDE_DIRS := $(BASE_DIR) $(CONN_DIR)

BASE_FILES := $(addprefix $(BASE_DIR)/, main.cpp)
CONN_FILES := $(addprefix $(CONN_DIR)/, TcpServer.cpp)
MODULE_FILES := $(CONN_FILES) # add other module files here
FILES := $(BASE_FILES) $(MODULE_FILES) # files to create main executable

OBJ_DIR := ./obj/
OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.cpp, %.o, $(FILES)))

DEP_FLAGS := -MP -MD
VPATH := $(MANDATORY_DIR)
CFLAGS := -g3 -std=c++98 -Wall -Wextra -Werror
CC := c++

NAME := webserver
INCLUDES := $(addprefix -I, $(INCLUDE_DIRS))
ETAGS_BASE := $(BASE_DIR)

DEP_FILES := $(patsubst %.o, %.d, $(OBJS))

all: $(NAME)

$(NAME): $(OBJS)
	-etags --language-force=c++ $$(find $(BASE_DIR) -name '*.[tch]pp')
	$(CC) $(CFLAGS) $^ $(INCLUDES) -o $@

$(OBJS): $(OBJ_DIR)%.o : %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) $(DEP_FLAGS) -o $@ -c $< $(INCLUDES)

$(OBJ_DIR):
	@mkdir -p $@



test_tcp: $(MODULE_FILES) $(TEST_DIR)/test_tcp.cpp $(TEST_DIR)/test_tcp.sh
	$(CC) $(CFLAGS) $(TEST_DIR)/test_tcp.cpp -o $@ $< $(INCLUDES)

clean:
	rm -fr $(OBJ_DIR) **/*~ *~ **/.#*

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus test_tcp

-include $(DEP_FILES)
