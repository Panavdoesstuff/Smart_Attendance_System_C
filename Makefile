app: main.o auth.o data.o utils.o
	gcc main.o auth.o data.o utils.o -o app

main.o: main.c auth.h data.h utils.h
	gcc -c main.c

auth.o: auth.c auth.h utils.h
	gcc -c auth.c

data.o: data.c data.h auth.h utils.h
	gcc -c data.c

utils.o: utils.c utils.h
	gcc -c utils.c

clean:
	rm -f *.o app