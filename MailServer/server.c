#include <Winsock2.h>
#include "status.h"
#include "buffer.h"
#include "codes.h"
#include "smtp_request.h"
#include "server.h"
#include "net.h"
#include "server_session.h"
#include "process.h"

#include "smtp_data.h"
#include "smtp_mail_from.h"
#include "smtp_rcpt_to.h"
#include "smtp_helo.h"
#include "smtp_quit.h"
#include "smtp_noop.h"
#include "smtp_rset.h"

static enum STATUS set_state(enum SERVER_STATE* current_state, enum SERVER_STATE state) {
	*current_state = state;
	return STATUS_OK;
}

static enum STATUS validate_state(enum SERVER_STATE current_state, enum SERVER_STATE state) {
	if (current_state < state) {
		return STATUS_NOT_OK;
	}
	return STATUS_OK;
}

static enum STATUS initialize_session(struct smtp_request** smtp_request, enum SERVER_STATE* current_state) {
	clean_smtp_request(*smtp_request);
	*smtp_request = init_smtp_request();
	set_state(current_state, INITIALIZED);
	return STATUS_OK;
}

void serve_connection(SOCKET sock) {
	int status = 0;
	enum SERVER_STATE current_state = DEFAULT;

	char* buffer = init_socket_buffer();

	struct smtp_request* smtp_request = init_smtp_request();
	struct server_session* server_session = init_server_session();

	send_response(sock, buffer, SERVICE_READY);

	while (1) {

		status = get_message(sock, buffer);
		if (status == STATUS_ERROR) break;

		lower_buffer(buffer);

		if (buffer_has_command("quit", buffer)) {
			serve_quit(sock, buffer);
			break;
		}

		if (buffer_has_command("noop", buffer)) {
			serve_noop(sock, buffer);
			continue;
		}

		if (buffer_has_command("rset", buffer)) {
			if (!validate_state(current_state, INITIALIZED)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_rset(sock, buffer);
			if (status == STATUS_OK) {
				initialize_session(&smtp_request, &current_state);
			}
			continue;
		}

		if (buffer_has_command("helo ", buffer)) {
			status = serve_helo(sock, buffer, server_session);
			if (status == STATUS_OK) {
				initialize_session(&smtp_request, &current_state);
			}
			continue;
		}

		if (buffer_has_command("mail from:", buffer)) {
			if (!validate_state(current_state, INITIALIZED)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_mail_from(sock, buffer, smtp_request);
			if (status == STATUS_OK) { 
				set_state(&current_state, HAS_MAIL_FROM);
			}
			continue;
		}

		if (buffer_has_command("rcpt to:", buffer)) {
			if (!validate_state(current_state, HAS_MAIL_FROM)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_rcpt_to(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				set_state(&current_state, HAS_RCPT_TO);
			}
			continue;
		}

		if (buffer_has_command("data", buffer)) {
			if (!validate_state(current_state, HAS_RCPT_TO)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_data(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				process_smtp_request(smtp_request, server_session);

				initialize_session(&smtp_request, &current_state);
			}
			continue;
		}

		send_response(sock, buffer, SYNTAX_ERROR);
	}
	clean_smtp_request(smtp_request);
	clean_server_session(server_session);
	clean_socket_buffer(buffer);
	socket_cleanup(sock);
}