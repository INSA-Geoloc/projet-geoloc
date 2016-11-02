#Un Makefile
CC = gcc

DATAFLAGS = -c -O3 -lm

DEBUGFLAGS = -c -g -O3 -Wall

GTKFLAGS = `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags cairo`


main: parcours_list.o data.o main.o
	$(CC) parcours_list.o data.o main.o -o main -lm

test: test.o graphic.o parcours_list.o data.o
	$(CC) test.o graphic.o parcours_list.o data.o -o test $(GTKFLAGS)

graphic.o: graphic.c graphic.h parcours_list.h data.h
	$(CC) $(DATAFLAGS) graphic.c $(GTKFLAGS)

test.o: test.c
	$(CC) $(DATAFLAGS) test.c $(GTKFLAGS)	

parcours_list.o: parcours_list.c parcours_list.h data.h
	$(CC) $(DATAFLAGS) parcours_list.c

data.o: data.c data.h parcours_list.h
	$(CC) $(DATAFLAGS) data.c

main.o: main.c
	$(CC) $(DATAFLAGS) main.c

clean:
	rm -rf main *.o
