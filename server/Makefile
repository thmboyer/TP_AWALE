bin/prog.out:bin/server.o bin/server_client.o bin/commands.o bin/server_package.o bin/game.o
	gcc -o bin/prog.out bin/server.o bin/server_client.o bin/commands.o bin/server_package.o bin/game.o

bin/server.o:server.c
	gcc -c server.c -o bin/server.o

bin/server_client.o:server_client.c
	gcc -c server_client.c -o bin/server_client.o

bin/commands.o:commands.c
	gcc -c commands.c -o bin/commands.o

bin/server_package.o:server_package.c
	gcc -c server_package.c -o bin/server_package.o

bin/game.o:game.c
	gcc -c game.c -o bin/game.o

clean:
	rm bin/*
