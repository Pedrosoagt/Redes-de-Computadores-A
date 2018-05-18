main:	main.o list.o
	gcc -g main.o list.o -o main -pthread

main.o:	main.c
	gcc main.c -c

list.o:	list.c list.h
	gcc list.c -c

clean:
	rm *.o
	clear
