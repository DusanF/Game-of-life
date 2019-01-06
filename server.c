#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include "defs.h"

const char* msg_type[] = {
	"Save",
	"Load",
	"User",
	"Filename"
};


enum {
	STAT_LOGIN_OK = 1,
	STAT_FILE_OK = 2,
	STAT_REWRITE = 4,
	STAT_FILE_UNLOCKED = 8
};


void printArray(char* name, void* array, unsigned size){
	char (*arr) = array;
	printf("\n");
	for(unsigned i=0; i<size; i++)
		printf("%s[%d]: 0x%02X = %3d = \'%c\'\n", name, i, *(arr+i), *(arr+i), *(arr+i));
}



char saveFile(char* filename, void* dat, uint16_t len){
	char (*size) = dat;
	uint16_t (*data) = dat+2;

	FILE *file;

	printf("Ukladam do: %s\n", filename);
	file = fopen(filename, "w");

	if(file == 0)
		return SERVER_RESP_ERR;

	fprintf(file, "%u\n%u\n", *size, *(size+1));
	for(unsigned i = 0; i < len/2; i++){
		fprintf(file, "%u\n", *(data+i));
	}
	fclose(file);

	return SERVER_RESP_OK;
}

char *loadFile(char *filename, unsigned *size){
	FILE *file;

	printf("Nacitavam z \"%s\"\n", filename);
	file = fopen(filename, "r");
	if(file == 0){
		printf("Subor sa nepodarilo otvorit!\n");
		return NULL;
	}

	unsigned readVal;
	char *buffer = malloc(4);

	fscanf(file, "%u", &readVal);
	*(buffer + 2) = readVal;
	fscanf(file, "%u", &readVal);
	*(buffer + 3) = readVal;

	unsigned i=4;

	while(fscanf(file, "%u", &readVal) != EOF){
		buffer = realloc(buffer, i+2);
		*(buffer + i++) = readVal;
		*(buffer + i++) = readVal >> 8;
	}

	fclose(file);

	*size = i;
	*buffer = i-2;
	*(buffer+1) = (i-2) >> 8;

	return buffer;
}

int main(int argc, char *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("Cakam na pripojenie\n");
	if((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen)) < 0){
		perror("accept");
		exit(EXIT_FAILURE);
	}
	printf("Pripojene\n");


	char cmd;
	uint16_t len;
	char *username;
	char *filename;
	char *buffer;
	char response;
	char status = 0;
	struct stat st = {0};

	while(1){
		response = SERVER_RESP_ERR;
		read(new_socket, &cmd, 1);
		printf("\n%s\n", msg_type[cmd]);

		switch(cmd){
			case SERVER_CMD_USER:
				read(new_socket, &len, 2);
				username = malloc(len + 1);
				read(new_socket, username, len);
				*(username + len) = 0;
				printf("Pouzivatel: %s\n", username);

				char *dirname = malloc(7 + strlen(username));
				strcpy(dirname, "saves/");
				strcat(dirname, username);
				if (stat(dirname, &st) == -1) {
					mkdir(dirname, 0700);
					response = SERVER_RESP_NEW;
				}else
					response = SERVER_RESP_OK;
				free(dirname);
				status |= STAT_LOGIN_OK;

				write(new_socket, &response, 1);
				break;


			case SERVER_CMD_FILENAME:
				read(new_socket, &len, 2);
				if(!(status & STAT_LOGIN_OK)){						//ak uzivatel nieje prihlaseny, len zahod prijaty nazov suboru
					filename = malloc(len);
					read(new_socket, filename, len);
					free(filename);
					break;
				}
				buffer = malloc(1+len);
				read(new_socket, buffer, len);
				*(buffer + len) = 0;

				filename = malloc(12 + len + strlen(username));
				strcpy(filename, "saves/");
				strcat(filename, username);
				strcat(filename, "/");
				strcat(filename, buffer);
				strcat(filename, ".gol");
				status |= STAT_FILE_OK;

				free(buffer);

				printf("Subor: %s\n", filename);

				if (stat(filename, &st) == -1){
					response = SERVER_RESP_FILE_NONEXISTS;
					status |= STAT_FILE_UNLOCKED;
				}else
					response = SERVER_RESP_FILE_EXISTS;

				write(new_socket, &response, 1);
				break;


			case SERVER_CMD_SAVE:
				read(new_socket, &len, 2);
				buffer = malloc(2 + len);
				unsigned act_read, buffpos=0;

				do{
					act_read = read(new_socket, buffer+buffpos, (2+len) - buffpos);
					printf("precitane: %u\n", act_read);
					buffpos += act_read;
				} while(buffpos < (2 + len));

				if(!(status & STAT_FILE_UNLOCKED)){
					printf("Subor neodomknuty, zahadzujem!\n");
					free(buffer);
					write(new_socket, &response, 1);
					break;
				}
				printf("ukladam\n");
				response = saveFile(filename, buffer, len);
				free(filename);
				free(username);
				free(buffer);
				write(new_socket, &response, 1);
				close(new_socket);
				return 0;
				break;

			case SERVER_CMD_LOAD:
				if(status != (STAT_LOGIN_OK | STAT_FILE_OK)){
					printf("Chyba nacitavania zo suboru\n");
					write(new_socket, &response, 1);
				}else{
					response = SERVER_RESP_OK;
					write(new_socket, &response, 1);

					unsigned size;
					buffer = loadFile(filename, &size);

					write(new_socket, buffer, size);

					free(filename);
					free(username);
					free(buffer);
					close(new_socket);
					return 0;
				}
				break;
		}
	}
	return 0;
}
