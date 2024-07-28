#include <WinSock2.h>
#include "net.h"

void clear_buffer(char* buffer) {
	memset(buffer, 0, BUFFER_SIZE);
}

void socket_cleanup(SOCKET sock) {
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

int get_message(SOCKET sock, char* buffer) {
	clear_buffer(buffer);
	int status = recv(sock, buffer, BUFFER_SIZE, 0);
	return status;
}

int send_message(SOCKET sock, char* response) {
	int status = send(sock, response, strlen(response), 0);
	clear_buffer(response);
	return status;
}

int send_response(SOCKET sock, char* response, char* code) {
	clear_buffer(response);
	add_to_buffer(response, code);
	return send_message(sock, response);
}

int get_message_length(char* buffer) {
	return strlen(buffer) - strlen("\r\n");
}