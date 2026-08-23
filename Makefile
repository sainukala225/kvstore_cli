.SILENT :

CC = gcc
CFLAGS = -Wall -Wextra -pthread -fsanitize=address,undefined -std=gnu23 -g -Iinclude/
TARGET = build/bin/kvstore
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,build/obj/%.o,$(SRCS))

all : $(TARGET)
	echo "build finished"

$(TARGET) : $(OBJS)
	mkdir -p $(dir $(TARGET))
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

build/obj/%.o : src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

run : $(TARGET)
	./$(TARGET)

test : $(TARGET)
	./tests/run.sh

clean :
	rm -rf build
	echo "clean finished"

