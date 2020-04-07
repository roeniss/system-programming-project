CC=gcc
CFLAGS=-Wall
OBJS=lib/20140424.o lib/myUtils.o lib/help.o lib/dir.o lib/quit.o lib/history.o lib/dump.o lib/opcode.o lib/reset.o lib/debug.o lib/edit.o lib/fill.o
TARGET=20140424.out

$(TARGET): $(OBJS)
	gcc -o $@ $(OBJS)

lib/20140424.o: 20140424.c
	$(CC) $(CFLAGS) -c -o $@ 20140424.c

## macro ##
clean:
	rm -f ./lib/*.o
	rm -f $(TARGET)
