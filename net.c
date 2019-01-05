#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include <pwd.h>
#include "defs.h"


void printArray(char* name, void* array, unsigned size){
	char (*arr) = array;
	printf("\n");
	for(unsigned i=0; i<size; i++)
		printf("%s[%d]: 0x%02X = %3d = \'%c\'\n", name, i, *(arr+i), *(arr+i), *(arr+i));
}


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

int net_login(int socket, char *username){
	char *buffer;

	buffer = malloc(3+strlen(username));

	*buffer = SERVER_CMD_USER;
	*(buffer+1) = (uint16_t) strlen(username);
	memcpy(buffer+3, username, strlen(username));

	write(socket, buffer, 3+strlen(buffer+3));

	free(buffer);

	char response;
	read(socket, &response, 1);

	return response;
}

int net_setFileName(int socket, char* filename){
	char *buffer;
	buffer = malloc(3 + strlen(filename));

	*buffer = SERVER_CMD_FILENAME;
	*(buffer+1) = (uint16_t) strlen(filename);
	memcpy(buffer+3, filename, strlen(filename));
	write(socket, buffer, 3+strlen(buffer+3));

	free(buffer);

	char response;
	read(socket, &response, 1);

	return response;
}

int net_sendWorld(int socket, void *w){
	world_t (*world) = w;

	char* buffer = malloc(7);								//minimalna mozna velkost: 1B-CMD + 1B-sirka + 1B-vyska + 2B-celkova dlzka + 2B-pocet mrtvych buniek

	*buffer = SERVER_CMD_SAVE;
	*(buffer+3) = (uint8_t) world->w;
	*(buffer+4) = (uint8_t) world->h;

	uint16_t series = 0;
	char cell_state = 0;
	uint16_t buff_ptr = 5;

	for(unsigned i=0; i<(world->w * world->h); i++){
		if(*(world->cells + i) != cell_state){
			*(buffer + buff_ptr++) = series;			//pocet rovnakych posebe iducich buniek
			*(buffer + buff_ptr++) = series >> 8;
			buffer = realloc(buffer, 2 + buff_ptr);
			series = 1;
			cell_state = *(world->cells + i);
		} else
			series++;
	}
	*(buffer + buff_ptr) = series;					//Zostavajuce bunky do konca sveta
	*(buffer + buff_ptr + 1) = series >> 8;

	*(buffer + 1) = buff_ptr - 3;				//pocet zmien x2 - vysledna velkost sveta po "komprimacii"
	*(buffer + 2) = (buff_ptr - 3) >> 8;
	
	write(socket, buffer, buff_ptr + 2);
	printArray("data", buffer, buff_ptr + 2);

	free(buffer);

	char response;
	read(socket, &response, 1);

	return response;
}

void net_save(void *w){
	char addr[18];
	int socket;

	printf("Zadaj IP adresu servera [localhost] : ");		//pripojenie ku serveru
	fgets(addr, 18, stdin);
	addr[strcspn(addr, "\n")] = 0;
	if(!addr[0])
		strcpy(addr, "127.0.0.1");

	if(net_connect(&socket, PORT, addr) != 1){
		sleep(2);
		return;
	}
	printf("Pripojenie uspesne\n");


	register struct passwd *pw;
	pw = getpwuid(geteuid());

	char ret = net_login(socket, pw->pw_name);				//prihlasenie na server
	if(ret == SERVER_RESP_OK)
		printf("Pouzivatel \"%s\" prihlaseny\n", pw->pw_name);
	else if(ret == SERVER_RESP_NEW)
		printf("Vytvoreny novy pouzivatel \"%s\"\n", pw->pw_name);


	printf("Zadaj nazov suboru: ");							//nazov suboru
	char *filename = malloc(FILENAME_MAX);
	fgets(filename, FILENAME_MAX, stdin);
	if(*(filename) == 0 || *(filename) == '\n')
		strcpy(filename, "new_save");
	filename[strcspn(filename, "\n")] = 0;

	ret = net_setFileName(socket, filename);
	free(filename);
	switch(ret){
		case SERVER_RESP_OK:
			printf("Vzdialeny subor vytvoreny\n");
			break;
		case SERVER_RESP_ERR:
			printf("Chyba pri vytvarani suboru\n");
			return;
			break;
		case SERVER_RESP_USED:
			printf("Subor uz existuje, prepisujem!\n");
			break;
		default:
			printf("Neznama chyba!\n");
			return;
			break;
	}


	ret = net_sendWorld(socket, w);							//odoslanie aktualneho stavu
	if(ret = SERVER_RESP_OK)
		printf("Ukladanie uspesne\n");
	else
		printf("Chyba pri ukladani\n");
	close(socket);
}
