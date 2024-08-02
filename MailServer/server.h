#pragma once

enum SERVER_STATES {
	DEFAULT = 0,
	INITIALIZED = 1,
	HAS_MAIL_FROM = 2,
	HAS_RCPT_TO = 3,
};

void serve_connection(SOCKET* sock);