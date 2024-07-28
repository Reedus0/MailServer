#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include "buffer.h"
#include "codes.h"
#include "smtp_request.h"
#include "server.h"
#include "net.h"

#include "smtp_data.h"
#include "smtp_mail_from.h"
#include "smtp_rcpt_to.h"
#include "smtp_helo.h"

enum STATUS validate_state(enum server_states current_state, enum server_states deserved_state) {
	return current_state & deserved_state;
}

enum STATUS validate_without_args(char* request, char* command) {
	int message_length = get_message_length(request);

	if (message_length != strlen(command)) {
		return STATUS_NOT_OK;
	}

	return STATUS_OK;
}

enum STATUS validate_with_args(char* request, char* command, char* separator) {
	int message_length = get_message_length(request);

	if (message_length == strlen(command)) {
		return STATUS_NOT_OK;
	}

	char* buffer = get_value_from_buffer(request, separator);
	buffer = trim_string(buffer);

	if (is_empty_string(buffer)) {
		return STATUS_NOT_OK;
	}

	free(buffer);

	return STATUS_OK;
}

static enum STATUS serve_quit(SOCKET sock, char* buffer) {
	if (validate_without_args(buffer, "QUIT") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	send_response(sock, buffer, SERVICE_CLOSING_TRANSMISSION);
	return STATUS_OK;
}

static enum STATUS serve_noop(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	if (validate_without_args(buffer, "NOOP") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	send_response(sock, buffer, ACTION_OK);
	return STATUS_OK;
}

static enum STATUS serve_rset(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	if (validate_without_args(buffer, "RSET") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	clean_smtp_request(smtp_request);

	send_response(sock, buffer, ACTION_OK);
	return STATUS_OK;
}

void serve_connection(SOCKET sock) {
	int status = 0;
	int current_state = DEFAULT;

	char* buffer = init_buffer();

	send_response(sock, buffer, SERVICE_READY);

	struct smtp_request* smtp_request = init_smtp_request();

	while (1) {

		status = get_message(sock, buffer);
		if (status == STATUS_ERROR) break;

		if (buffer_has_command("QUIT", buffer)) {
			serve_quit(sock, buffer);
			break;
		}

		if (buffer_has_command("NOOP", buffer)) {
			serve_noop(sock, buffer, smtp_request);
			continue;
		}

		if (buffer_has_command("RSET", buffer)) {
			status = serve_rset(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				smtp_request = init_smtp_request();
				current_state = DEFAULT;
			}
			continue;
		}

		if (buffer_has_command("HELO ", buffer)) {
			serve_helo(sock, buffer, smtp_request);
			continue;
		}

		if (buffer_has_command("MAIL FROM:", buffer)) {
			status = serve_mail_from(sock, buffer, smtp_request);
			if (status == STATUS_OK) { 
				current_state |= HAS_FROM;
			}
			continue;
		}

		if (buffer_has_command("RCPT TO:", buffer)) {
			if (!validate_state(current_state, HAS_FROM)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_rcpt_to(sock, buffer, smtp_request, current_state);
			if (status == STATUS_OK) {
				current_state |= HAS_TO;
			}
			continue;
		}

		if (buffer_has_command("DATA", buffer)) {
			if (!validate_state(current_state, HAS_FROM | HAS_TO)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_data(sock, buffer, smtp_request, current_state);
			if (status == STATUS_OK) {
				smtp_request = init_smtp_request();
				current_state = DEFAULT;
			}
			continue;
		}

		send_response(sock, buffer, SYNTAX_ERROR);
	}
	clean_smtp_request(smtp_request);
	socket_cleanup(sock);
	clean_buffer(buffer);
}