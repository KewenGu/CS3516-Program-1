all: MEClient MEServer

MEServer: MEServer.o helper.o
	gcc -g MEServer.o helper.o -o MEServer

MEServer.o: MEServer.c header.h
	gcc -g -c MEServer.c

MEClient: MEClient.o helper.o
	gcc -g MEClient.o helper.o -o MEClient

MEClient.o: MEClient.c header.h
	gcc -g -c MEClient.c

helper.o: helper.c header.h
	gcc -g -c helper.c

clean:
	rm -f *.o MEClient MEServer 