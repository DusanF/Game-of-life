#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "defs.h"
#include <pwd.h>

int net_connect(int *sck, int port, char *adr) {
	struct sockaddr_in serv_addr;

	if((*sck = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Chyba pri vytvarani soketu\n");
		return -1;
		}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, adr, &serv_addr.sin_addr)<=0){
		perror("Zla alebo nepodporovana adresa\n");
		return -1;
		}

	if(connect(*sck, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Pripajanie neuspesne\n");
		return -1;
		}
	return 1;
	}


void printBuff(char* buff, unsigned size){
	printf("\n\n");
	for(unsigned i=0; i<size; i++)
		printf("buffer[%d]: 0x%02X = %3d = \'%c\'\n", i, buff[i], buff[i], buff[i]);
}


void net_save(void *w){
	world_t (*world) = w;

	char addr[16];
	int port;
	int socket;

	printf("Zadaj IP adresu servera: ");
	scanf("%s", addr);
	printf("Zadaj cielovy port: ");
	scanf("%d", &port);
	if(net_connect(&socket, port, addr) != 1){
		sleep(2);
		return;
	}
	printf("Pripojenie uspesne\n");
	
	char *buffer;
	register struct passwd *pw;
	pw = getpwuid(geteuid());
	
	buffer = malloc(strlen(pw->pw_name));
	strcpy(buffer, pw->pw_name);
	write(socket, buffer, strlen(buffer));

	char odpoved;
	read(socket, &odpoved, 1);
	if(odpoved == SERVER_USER_OK)
		printf("Pouzivatel \"%s\" prihlaseny\n", buffer);
	else if(odpoved == SERVER_USER_NEW)
		printf("Vytvoreny novy pouzivatel \"%s\"\n", buffer);
	
	printf("Zadaj nazov suboru: ");

	buffer = realloc(buffer, 30);
	scanf("%s", buffer+3);
	
	unsigned buffSize = strlen(buffer+3) + 3 + world->w*world->h;
	*buffer = world->w;
	*(buffer+1) = world->h;
	*(buffer+2) = strlen(buffer+3);
	
	buffer = realloc(buffer, buffSize);
	memcpy(buffer+3+strlen(buffer+3), world->cells, world->w*world->h);

	write(socket, buffer, buffSize);
	free(buffer);

	read(socket, &odpoved, 1);
	if(odpoved = SERVER_SAVE_OK){
		printf("Ukladanie uspesne\n");
	}else{
		printf("Chyba pri ukladani\n");
	}
}
