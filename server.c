#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "defs.h"

#define PORT 1234



int main(int argc, char *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char *buffer;

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

	char response;
	unsigned w, h;
	char* filename;
	char* username;
	char* cells;

	buffer = malloc(31);
	recv(new_socket, buffer, 30, 0);
	buffer[30] = 0;

	username = malloc(strlen(buffer));
	strcpy(username, buffer);

	struct stat st = {0};
	if (stat(username, &st) == -1) {
		mkdir(username, 0700);
		response = SERVER_USER_NEW;
	}
	else
		response = SERVER_USER_OK;

	send(new_socket, &response, 1, 0);

	buffer = realloc(buffer, 3);
	recv(new_socket, buffer, 3, 0);

	w = *buffer;
	h = *(buffer + 1);
	unsigned allocated = *(buffer + 2);
	buffer = realloc(buffer, allocated);

	recv(new_socket, buffer, allocated, 0);
	filename = malloc(allocated + 1);
	memcpy(filename, buffer, allocated);
	*(filename + allocated) = 0;

	allocated = w*h;
	buffer = realloc(buffer, allocated);
	cells = malloc(allocated);

	recv(new_socket, buffer, allocated, 0);
	memcpy(cells, buffer, allocated);
	free(buffer);

	response = SERVER_SAVE_OK;
	send(new_socket, &response, 1, 0);

	printf("\n%s:\n", filename);
	for(int y=0; y<h; y++){
		printf("\n");
		for(int x=0; x<w; x++)
			if(*(cells+x+(y*w))){
				putchar('{');
				putchar('}');
			}else{
				putchar(' ');
				putchar(' ');
			}
	}
	printf("\n");


	free(cells);
	free(username);
	free(filename);
	return 0;
}
