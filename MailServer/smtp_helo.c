#include <string.h>
#include <WinSock2.h>
#include "smtp_data.h"
#include "codes.h"
#include "header.h"
#include "server.h"
#include "net.h"
#include "buffer.h"
#include "smtp_request.h"

enum STATUS serve_helo(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	if (validate_with_args(buffer, "HELO ", " ") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	char* domain = get_value_from_buffer(buffer, " ");
	domain = trim_string(domain);

	smtp_request_set_domain(smtp_request, domain);

	send_response(sock, buffer, ACTION_OK);

	return STATUS_OK;
}