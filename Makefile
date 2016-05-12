server : server.o client.o shm.o
	gcc -o server server.o
	gcc -o client client.o

server.o : server.c
	gcc -c server.c util.h shm.c semaphore.c semaphore.h
client.o : client.c
	gcc -c client.c shm.c semaphore.c semaphore.h