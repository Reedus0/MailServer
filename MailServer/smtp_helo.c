#include <string.h>
#include <WinSock2.h>
#include "smtp_data.h"
#include "codes.h"
#include "header.h"
#include "server.h"
#include "net.h"
#include "buffer.h"
#include "smtp_request.h"
#include "validation.h"
#include "server_session.h"

enum STATUS serve_helo(SOCKET sock, char* buffer, struct server_session* server_session) {
	if (validate_with_args(buffer, "HELO ", " ") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	char* hostname = get_value_from_buffer(buffer, " ");
	hostname = trim_string(hostname);

	server_session_set_hostname(server_session, hostname);

	send_response(sock, buffer, ACTION_OK);

	return STATUS_OK;
}