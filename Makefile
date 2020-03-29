#reference : https://www.tuwlab.com/ece/27193

#### SAMPLE~ ####

# app.out: main.o foo.o bar.o
# 	gcc -o app.out main.o foo.o bar.o

# main.o: foo.h bar.h main.c
# 	gcc -c -o main.o main.c

# foo.o: foo.h foo.c
# 	gcc -c -o foo.o foo.c

# bar.o: bar.h bar.c
# 	gcc -c -o bar.o bar.c

#### ~SAMPLE ####

CC=gcc
CFLAGS=-Wall
OBJS=20140424.o myUtils.o help.o dir.o quit.o history.o dump.o opcode.o reset.o debug.o edit.o fill.o 
TARGET=20140424.out

$(TARGET): $(OBJS)
	gcc -o $@ $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ main.c

myUtils.o: myUtils.c
help.o: help.c
dir.o: dir.c
quit.o: quit.c
history.o: history.c
dump.o: dump.c
opcode.o: opcode.c
reset.o: reset.c
debug.o: debug.c
edit.o: edit.c
fill.o: fill.c



## macro ##
clean:
	rm -f *.o
	rm -f $(TARGET)
## ~macro ##