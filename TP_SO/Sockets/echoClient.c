/*
 * echoClient.c
 *
 *  Created on: 17/06/2014
 *      Author: utnso
 */
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	char server_reply[1025];
	char* message=malloc(1025);
	int valread;

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected\n");
	if ((valread = read(sock, server_reply, 1024)) == 0) {
		puts("recv failed");

	}
	server_reply[valread] = '\0';
	puts(server_reply);

	//keep communicating with server
	while (1) {
	gets(message);
		//Send some data
		if (send(sock, message, 1024, 0) == 0) {
			puts("Send failed");
			break;
			return 1;
		}

		//Receive a reply from the server
		if ((valread = read(sock, server_reply, 1024)) == 0) {
			puts("recv failed");
			break;

		}
		server_reply[valread] = '\0';
		puts(server_reply);
	}
	close(sock);
	return 0;
}

