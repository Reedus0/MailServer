#pragma once

#define BUFFER_SIZE 128

enum server_states {
	DEFAULT = 0,
	HAS_FROM = 1,
	HAS_TO = 2,
};

void serve_connection(SOCKET* sock);