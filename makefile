CC := gcc
C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))
LD_FLAGS := -lm -L/usr/local/lib -lncurses -lzlog -lcurl
CC_FLAGS := -Wall -g -I/usr/local/include

bin/alerts: $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $^ $(LD_FLAGS) -o $@

obj/%.o: src/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	rm obj/*
	rm bin/*
