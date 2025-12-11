# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bnespoli <bnespoli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/15 17:53:38 by maurodri          #+#    #+#              #
#    Updated: 2025/12/11 17:59:20 by maurodri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

BASE_DIR := ./src
TEST_DIR := ./test
CONN_DIR := $(addprefix $(BASE_DIR), /conn)
UTIL_DIR := $(addprefix $(BASE_DIR), /util)
HTTP_DIR := $(addprefix $(BASE_DIR), /http)
CONFIG_DIR := $(addprefix $(BASE_DIR), /config)
INCLUDE_DIRS := $(BASE_DIR) \
	$(CONN_DIR) \
	$(UTIL_DIR) \
	$(HTTP_DIR) \
	$(CONFIG_DIR) # add other module directories here

BASE_FILES := $(addprefix $(BASE_DIR)/, main.cpp)
CONN_FILES := $(addprefix $(CONN_DIR)/, TcpServer.cpp \
					TcpClient.cpp \
					Operation.cpp \
					EventLoop.cpp)
UTIL_FILES := $(addprefix $(UTIL_DIR)/, BufferedReader.cpp\
					BufferedWriter.cpp \
					pathUtils.cpp)

HTTP_FILES := $(addprefix $(HTTP_DIR)/, Request.cpp \
					Headers.cpp \
					Body.cpp \
					Client.cpp \
					Response.cpp \
					Application.cpp \
					Server.cpp \
					VirtualServer.cpp \
					RequestPath.cpp \
					Route.cpp \
					RouteCgi.cpp \
					RouteStaticFile.cpp \
					Dispatcher.cpp)

CONFIG_FILES := $(addprefix $(CONFIG_DIR)/, ApplicationSpec.cpp \
					ServerSpec.cpp \
					VirtualServerSpec.cpp \
					Scanner.cpp \
					RouteSpec.cpp)

MODULE_FILES := $(CONN_FILES) \
	$(UTIL_FILES) \
	$(HTTP_FILES) \
	$(CONFIG_FILES)
# add other module files here

FILES := $(BASE_FILES) $(MODULE_FILES) # files to create main executable

OBJ_DIR := ./obj/
OBJS := $(addprefix $(OBJ_DIR), $(patsubst %.cpp, %.o, $(FILES)))

BUFFER_SIZE_ARG = $(if $(BUFFER_SIZE),-DBUFFER_SIZE=$(BUFFER_SIZE))

DEP_FLAGS := -MP -MD
VPATH := $(MANDATORY_DIR)
CFLAGS := -g3 -std=c++98 -Wall -Wextra -Werror $(BUFFER_SIZE_ARG)
CC := c++

NAME := webserver
INCLUDES := $(addprefix -I, $(INCLUDE_DIRS))
ETAGS_BASE := $(BASE_DIR)

DEP_FILES := $(patsubst %.o, %.d, $(OBJS))

all: $(NAME)

$(NAME): $(OBJS)
	-etags $$(find $(BASE_DIR) -name '*.[tch]pp')
	$(CC) $(CFLAGS) $^ $(INCLUDES) -o $@

$(OBJS): $(OBJ_DIR)%.o : %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEP_FLAGS) -o $@ -c $< $(INCLUDES)

$(OBJ_DIR):
	@mkdir -p $@


test_tcp: $(OBJS) $(TEST_DIR)/test_tcp.cpp $(TEST_DIR)/test_tcp.sh
	$(CC) $(CFLAGS) $(TEST_DIR)/test_tcp.cpp -o $@ $(MODULE_FILES) $(INCLUDES)

test_buff_read: $(OBJS) $(TEST_DIR)/test_bufferedReader_read.cpp $(TEST_DIR)/test_bufferedReader_read.sh
	$(CC) $(CFLAGS) $(TEST_DIR)/test_bufferedReader_read.cpp -o $@ $(MODULE_FILES) $(INCLUDES)

test_buff_readlineCrlf: $(OBJS) $(TEST_DIR)/test_bufferedReader_readlineCrlf.cpp $(TEST_DIR)/test_bufferedReader_readlineCrlf.sh
	$(CC) $(CFLAGS) $(TEST_DIR)/test_bufferedReader_readlineCrlf.cpp -o $@ $(MODULE_FILES) $(INCLUDES)

test_end_to_end: $(OBJS) $(TEST_DIR)/test_end_to_end.cpp $(TEST_DIR)/test_end_to_end.sh
	$(CC) $(CFLAGS) $(TEST_DIR)/test_end_to_end.cpp -o $@ $(MODULE_FILES) $(INCLUDES)

clean:
	rm -fr $(OBJ_DIR) **/*~ *~ **/.#*

fclean: clean
	-rm ./test_*
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus test_tcp

-include $(DEP_FILES)
