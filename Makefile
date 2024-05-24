all:
	gcc -o proxy multi-proxy.c fonction.c
	gcc -o client multi-client.c fonction.c
	gcc -o serveur multi-serveur.c fonction.c
clean:
	rm -f proxy client serveur
	rm -f *.o