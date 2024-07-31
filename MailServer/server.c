#include <Winsock2.h>
#include "header.h"
#include "buffer.h"
#include "codes.h"
#include "smtp_request.h"
#include "server.h"
#include "net.h"
#include "server_session.h"
#include "validation.h"
#include "process.h"

#include "smtp_data.h"
#include "smtp_mail_from.h"
#include "smtp_rcpt_to.h"
#include "smtp_helo.h"
#include "smtp_quit.h"
#include "smtp_noop.h"
#include "smtp_rset.h"

static enum STATUS initialize_session(struct smtp_request** smtp_request, struct server_session* server_session) {
	clean_smtp_request(*smtp_request);
	*smtp_request = init_smtp_request();
	smtp_request_set_session(*smtp_request, server_session);
	server_session_set_state(server_session, INITIALIZED);
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

		lower_buffer(buffer);

		if (buffer_has_command("quit", buffer)) {
			serve_quit(sock, buffer);
			break;
		}

		if (buffer_has_command("noop", buffer)) {
			serve_noop(sock, buffer, smtp_request);
			continue;
		}

		if (buffer_has_command("rset", buffer)) {
			status = serve_rset(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				initialize_session(&smtp_request, server_session);
			}
			continue;
		}

		if (buffer_has_command("helo ", buffer)) {
			status = serve_helo(sock, buffer, server_session);
			if (status == STATUS_OK) {
				initialize_session(&smtp_request, server_session);
			}
			continue;
		}

		if (buffer_has_command("mail from:", buffer)) {
			if (!server_session_validate_state(server_session, INITIALIZED)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}
			status = serve_mail_from(sock, buffer, smtp_request);
			if (status == STATUS_OK) { 
				server_session_set_state(server_session, HAS_MAIL_FROM);
			}
			continue;
		}

		if (buffer_has_command("rcpt to:", buffer)) {
			if (!server_session_validate_state(server_session, HAS_MAIL_FROM)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_rcpt_to(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				server_session_set_state(server_session, HAS_RCPT_TO);
			}
			continue;
		}

		if (buffer_has_command("data", buffer)) {
			if (!server_session_validate_state(server_session, HAS_RCPT_TO)) {
				send_response(sock, buffer, BAD_SEQUENCE);
				continue;
			}

			status = serve_data(sock, buffer, smtp_request);
			if (status == STATUS_OK) {
				process_smtp_request(smtp_request);

				initialize_session(&smtp_request, server_session);
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