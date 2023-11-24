bin/prog.out:bin/server.o bin/server_client.o
	gcc -o bin/prog.out bin/server.o bin/server_client.o

bin/server.o:server.c
	gcc -c server.c -o bin/server.o

bin/server_client.o:server_client.c
	gcc -c server_client.c -o bin/server_client.o

clean:
	rm bin/*
