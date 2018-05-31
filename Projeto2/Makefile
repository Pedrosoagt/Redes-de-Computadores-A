serv:	serv.o contacts.o
	gcc -g serv.o contacts.o -o serv -pthread

serv.o:	serv.c
	gcc serv.c -c

list.o:	contacts.c contacts.h
	gcc contacts.c -c

clean:
	rm *.o
	clear
