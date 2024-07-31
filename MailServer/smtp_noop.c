#include <WinSock2.h>
#include "validation.h"
#include "status.h"
#include "net.h"
#include "codes.h"

enum STATUS serve_noop(SOCKET sock, char* buffer) {
	if (validate_without_args(buffer, "noop") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	send_response(sock, buffer, ACTION_OK);
	return STATUS_OK;
}