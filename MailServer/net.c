#include <WinSock2.h>
#include "net.h"

char* init_socket_buffer() {
	char* new_buffer = calloc(BUFFER_SIZE, sizeof(char));
	return new_buffer;
}

void clear_socket_buffer(char* buffer) {
	memset(buffer, 0, BUFFER_SIZE);
}

void socket_cleanup(SOCKET sock) {
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

int get_message(SOCKET sock, char* buffer) {
	clear_socket_buffer(buffer);
	int status = 0;
	while (1) {
		status = recv(sock, buffer + strlen(buffer), BUFFER_SIZE, 0);
		if (strstr(buffer, "\r\n")) {
			break;
		}
	}
	return status;
}

int send_message(SOCKET sock, char* response) {
	int status = send(sock, response, strlen(response), 0);
	clear_socket_buffer(response);
	return status;
}

int send_response(SOCKET sock, char* response, char* code) {
	clear_socket_buffer(response);
	add_to_buffer(response, code);
	return send_message(sock, response);
}

int get_message_length(char* buffer) {
	return strlen(buffer) - strlen("\r\n");
}

int clean_socket_buffer(char* buffer) {
	free(buffer);
}