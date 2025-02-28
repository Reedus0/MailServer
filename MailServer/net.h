#pragma once

#include <WinSock2.h>

#define BUFFER_SIZE 512

char* init_socket_buffer();
void clear_socket_buffer(char* buffer);
void socket_cleanup(SOCKET sock);
int get_message(SOCKET sock, char* buffer);
int send_message(SOCKET sock, char* response);
int send_response(SOCKET sock, char* response, char* code);
int get_message_length(char* buffer);
int clean_socket_buffer(char* buffer);