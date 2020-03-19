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
OBJS=main.o
TARGET=20140424.out

$(TARGET): $(OBJS)
	gcc -o $@ $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ main.c