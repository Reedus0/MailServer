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
#include "server_session.h"
#include "validation.h"

#include "smtp_data.h"
#include "smtp_mail_from.h"
#include "smtp_rcpt_to.h"
#include "smtp_helo.h"


static enum STATUS serve_noop(SOCKET sock, char* buffer) {
	if (validate_without_args(buffer, "NOOP") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	send_response(sock, buffer, ACTION_OK);
	return STATUS_OK;
}

static enum STATUS serve_quit(SOCKET sock, char* buffer) {
	return serve_noop(sock, buffer);
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

	char* buffer = init_buffer();

	send_response(sock, buffer, SERVICE_READY);

	struct smtp_request* smtp_request = init_smtp_request();
	struct server_session* server_session = init_server_session();

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
				smtp_request_set_session(smtp_request, server_session);
				server_session_set_state(server_session, HAS_DOMAIN);
			}
			continue;
		}

		if (buffer_has_command("HELO ", buffer)) {
			status = serve_helo(sock, buffer, server_session);
			if (status == STATUS_OK) {
				smtp_request_set_session(smtp_request, server_session);
				server_session_set_state(server_session, HAS_DOMAIN);
			}
			continue;
		}

		if (buffer_has_command("MAIL FROM:", buffer)) {
			if (!server_session_validate_state(server_session, HAS_DOMAIN)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}
			status = serve_mail_from(sock, buffer, smtp_request);
			if (status == STATUS_OK) { 
				server_session_set_state(server_session, HAS_FROM);
			}
			continue;
		}

		if (buffer_has_command("RCPT TO:", buffer)) {
			if (!server_session_validate_state(server_session, HAS_FROM)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_rcpt_to(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				server_session_set_state(server_session, HAS_TO);
			}
			continue;
		}

		if (buffer_has_command("DATA", buffer)) {
			if (!server_session_validate_state(server_session, HAS_TO)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_data(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				smtp_request = init_smtp_request();
				smtp_request_set_session(smtp_request, server_session);
				server_session_set_state(server_session, HAS_DOMAIN);
			}
			continue;
		}

		send_response(sock, buffer, SYNTAX_ERROR);
	}
	clean_smtp_request(smtp_request);
	clean_server_session(server_session);
	socket_cleanup(sock);
	clean_buffer(buffer);
}