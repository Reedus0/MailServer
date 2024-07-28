#pragma once

#define BUFFER_SIZE 128

char* init_buffer();
void clear_buffer(char* buffer);
void socket_cleanup(SOCKET sock);
int get_message(SOCKET sock, char* buffer);
int send_message(SOCKET sock, char* response);
int send_response(SOCKET sock, char* response, char* code);
int get_message_length(char* buffer);
int clean_buffer(char* buffer);