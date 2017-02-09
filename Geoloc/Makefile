#Un Makefile
CC = gcc

DATAFLAGS = -c -O3

DEBUGFLAGS = -c -g -O3 -Wall

GTKFLAGS = `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags cairo`

mainwindow: main_window.o graphic.o parcours_list.o data.o traitement-donnees.o
	$(CC) main_window.o graphic.o parcours_list.o data.o traitement-donnees.o -o mainwindow -lm $(GTKFLAGS)

main_window.o: main_window.c
	$(CC) $(DATAFLAGS) main_window.c $(GTKFLAGS)

graphic.o: graphic.c graphic.h parcours_list.h data.h
	$(CC) $(DATAFLAGS) graphic.c $(GTKFLAGS) -lm

test.o: test.c
	$(CC) $(DATAFLAGS) test.c $(GTKFLAGS) -lm

parcours_list.o: parcours_list.c parcours_list.h data.h
	$(CC) $(DATAFLAGS) parcours_list.c -lm

data.o: data.c data.h parcours_list.h
	$(CC) $(DATAFLAGS) data.c -lm

traitement-donnees.o: traitement-donnees.c traitement-donnees.h parcours_list.h data.h
	$(CC) $(DATAFLAGS) traitement-donnees.c -lm

clean:
	rm -rf main mainwindow *.o
