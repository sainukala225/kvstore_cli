.SILENT :

CC = gcc
BASEFLAGS = -Wall -Wextra -pthread -std=gnu23 -g -Iinclude/
SANFLAGS = -fsanitize=address,undefined
CFLAGS = $(BASEFLAGS) $(SANFLAGS)

TARGET = build/bin/kvstore
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,build/obj/%.o,$(SRCS))

PLAIN_TARGET = build/bin/kvstore-plain
PLAIN_OBJS := $(patsubst src/%.c,build/obj-plain/%.o,$(SRCS))

TEST_TARGET = build/bin/state_test
LIB_OBJS := $(filter-out build/obj/kvstore_cli.o,$(OBJS))

all : $(TARGET)
	echo "build finished"

$(TARGET) : $(OBJS)
	mkdir -p $(dir $(TARGET))
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

build/obj/%.o : src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(PLAIN_TARGET) : $(PLAIN_OBJS)
	mkdir -p $(dir $(PLAIN_TARGET))
	$(CC) $(BASEFLAGS) -o $(PLAIN_TARGET) $(PLAIN_OBJS)

build/obj-plain/%.o : src/%.c
	mkdir -p $(dir $@)
	$(CC) $(BASEFLAGS) -o $@ -c $<

run : $(TARGET)
	./$(TARGET)

$(TEST_TARGET) : tests/state.c $(LIB_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ tests/state.c $(LIB_OBJS)

test : $(TARGET) $(TEST_TARGET)
	./tests/run.sh

valgrind : $(PLAIN_TARGET)
	valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./$(PLAIN_TARGET)

analyze :
	for f in $(SRCS); do \
	  echo "analyzing $$f"; \
	  $(CC) $(BASEFLAGS) -fanalyzer -Wno-analyzer-malloc-leak -c -o /dev/null $$f || exit 1; \
	done
	echo "analyze finished"

clean :
	rm -rf build
	echo "clean finished"

