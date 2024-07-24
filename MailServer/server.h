#pragma once

void serve_connection(SOCKET* sock);

enum server_states {
	DEFAULT = 0,
	HAS_FROM = 1,
	HAS_TO = 2,
};