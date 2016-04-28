client : client.c util.h
	gcc -o client client.c

serveur : server.c util.h
	gcc -o server server.c
  
clean :
	rm client serveur