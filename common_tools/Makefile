CC = gcc
CFLAGS = -W -Wall
TARGET=20140424.out
OBJS = 20140424.o lib/history.o lib/assemble.o lib/optab.o lib/others.o lib/vm.o lib/symtab.o

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f ./lib/*.o
	rm -f $(TARGET)
