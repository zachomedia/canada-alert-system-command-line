SRC := src
OBJ := obj
BIN := bin

CC := gcc
C_FILES := $(wildcard $(SRC)/*.c)
OBJ_FILES := $(addprefix $(OBJ)/,$(notdir $(C_FILES:.c=.o)))
LD_FLAGS := -lm -L/usr/local/lib -lncurses -lcurl -g
CC_FLAGS := -Wall -g -I/usr/local/include -std=c99 -g

INSTALL := /usr/local/bin

build: bin/ obj/ alerts

bin/:
	mkdir bin

obj/:
	mkdir obj

alerts: $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $^ $(LD_FLAGS) -o $(BIN)/$@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

install:
	cp -r $(BIN)/* $(INSTALL)

.PHONY: clean
clean:
	rm -rf $(OBJ)/*
	rm -rf $(BIN)/*
