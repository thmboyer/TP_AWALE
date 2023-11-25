bin/prog.out:bin/client.o bin/client_package.o 
	gcc -o bin/prog.out bin/client.o bin/client_package.o

bin/client.o: client.c
	gcc -c client.c -o bin/client.o

bin/client_package.o:client_package.c
	gcc -c client_package.c -o bin/client_package.o

clean:
	rm bin/*
