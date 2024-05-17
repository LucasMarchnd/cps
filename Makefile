all:
	gcc -o serveur serveur-tcp.c fonction.c
	gcc -o client client-tcp.c fonction.c
	gcc -o proxy proxy-tcp.c fonction.c
clean:
	rm -f serveur client proxy
	rm -f *.o