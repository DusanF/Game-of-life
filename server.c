#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 1234


unsigned w, h;
char* name;
char* cells;

void printBuff(char* buff, unsigned size){	
	printf("\n%s:\n", name);
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
}

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
	
	buffer = malloc(3);
	recv(new_socket, buffer, 3, 0);
	w = *buffer;
	h = *(buffer + 1);
	unsigned allocated = *(buffer + 2);

	buffer = realloc(buffer, allocated);
	recv(new_socket, buffer, allocated, 0);
	name = malloc(allocated + 1);
	memcpy(name, buffer, allocated);
	*(name + allocated) = 0;

	allocated = w*h;
	buffer = realloc(buffer, allocated);
	cells = malloc(allocated);
	recv(new_socket, buffer, allocated, 0);
	memcpy(cells, buffer, allocated);
	free(buffer);

	char response = 1;
	send(new_socket, &response, 1, 0);
	printBuff(buffer, valread);
	free(cells);
	free(name);
	return 0;
}
