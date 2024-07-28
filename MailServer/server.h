#pragma once

enum server_states {
	DEFAULT = 0,
	HAS_FROM = 1,
	HAS_TO = 2,
};

enum STATUS validate_state(enum server_states current_state, enum server_states deserved_state);
enum STATUS validate_without_args(char* request, char* command);
enum STATUS validate_with_args(char* request, char* command, char* separator);
void serve_connection(SOCKET* sock);