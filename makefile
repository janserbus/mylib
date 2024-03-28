CC := gcc
CFLAGS := -Wall -Wextra
HEADERS := $(wildcard *.h)
LIB_DIR := lib
OBJ_DIR := obj
INC_DIR := include
SCRIPT_DIR := scripts
DATALIB_NAME := libdstructs.so
DATA_DIR := dstructs
DATA_SRC := $(wildcard $(DATA_DIR)/*.c)
DATA_OBJ := $(patsubst  $(DATA_DIR)/%.c,$(OBJ_DIR)/%.o,$(DATA_SRC))
RENDLIB_NAME := libtreerend.so
REND_DIR := treerend
REND_SRC := $(wildcard $(REND_DIR)/*.c)
REND_OBJ := $(patsubst $(REND_DIR)/%.c,$(OBJ_DIR)/%.o,$(REND_SRC))
REND_LINKS := -lSDL2 -lSDL2_ttf -lfontconfig -ldstructs

.PHONY: test

test: test.c
	$(CC) $(CFLAGS) -g3 -o $@ $^ -I$(INC_DIR) -L$(LIB_DIR) -ldstructs -ltreerend

#LIBRARY COMPILATION

.PHONY: all

all:$(LIB_DIR)/$(DATALIB_NAME) $(LIB_DIR)/$(RENDLIB_NAME)

#COMPILE TREEREND LIBRARY
$(LIB_DIR)/$(RENDLIB_NAME): $(REND_OBJ) 
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -shared -L$(LIB_DIR) -o $@ $^ $(REND_LINKS)

#COMPILE DSTRUCTS LIBRARY
$(LIB_DIR)/$(DATALIB_NAME): $(DATA_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -shared -o $@ $^

#COMPILE TREEREND OBJECT FILES
$(OBJ_DIR)/%.o: $(DATA_DIR)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -fPIC -I$(INC_DIR) -c $< -o $@

#COMPILE DSTRUCTS OBJECT FILES
$(OBJ_DIR)/%.o: $(REND_DIR)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -fPIC -I$(INC_DIR) -c $< -o $@ 

.PHONY: clean

clean:
	rm -rf $(LIB_DIR) $(OBJ_DIR)

# LIBRARY MANAGEMENT

.PHONY: install
.PHONY: uninstall

install: all
	sudo ./$(SCRIPT_DIR)/$@.sh
	make clean

uninstall:
	sudo ./$(SCRIPT_DIR)/$@.sh

#DEBUG COMPILATION

debug: d_list d_treerend d_tree
	$(CC) -Wall -Wextra -g3 -o test test.c list.o tree.o treerend.o -I . -lSDL2 -lSDL2_ttf -lfontconfig -lm

d_treerend: d_list
	$(CC) -Wall -Wextra -g3 -c $(TREEREND_FOLDER)/treerend.c -I .

d_list:
	$(CC) -Wall -Wextra -g3 -c $(DSTRUCTS_FOLDER)/list.c -I .

d_tree:
	$(CC) -Wall -Wextra -g3 -c $(DSTRUCTS_FOLDER)/tree.c -I .
