.SILENT :

CC = clang
CFLAGS = -Wall -Wextra -pthread -std=c99
TARGET = build/bin/kvstore
OBJS = build/obj/kvstore.o

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

clean :
	rm -rf build
	echo "clean finished"


