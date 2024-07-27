#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include "buffer.h"
#include "codes.h"
#include "smtp_request.h"
#include "delivery.h"
#include "server.h"
#include "email_address.h"
#include "config.h"

static void clear_buffer(char* buffer) {
	memset(buffer, 0, BUFFER_SIZE);
}

static void socket_cleanup(SOCKET sock) {
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

static int get_message(SOCKET sock, char* buffer) {
	clear_buffer(buffer);
	int status = recv(sock, buffer, BUFFER_SIZE, 0);
	return status;
}

static int send_message(SOCKET sock, char* response) {
	int status = send(sock, response, strlen(response), 0);
	clear_buffer(response);
	return status;
}

static int send_response(SOCKET sock, char* response, char* code) {
	clear_buffer(response);
	add_to_buffer(response, code);
	return send_message(sock, response);
}

static int get_message_length(char* buffer) {
	return strlen(buffer) - 2;
}

static int serve_quit(SOCKET sock, char* buffer) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (message_length != strlen("QUIT")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	status = send_response(sock, buffer, SERVICE_CLOSING_TRANSMISSION);
	if (status == -1) return -1;
	return 1;
}

static int serve_helo(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (message_length == strlen("HELO ")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* domain = get_value_from_buffer(buffer, " ");
	domain = trim_string(domain);

	if (is_empty_string(domain)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	smtp_request_set_domain(smtp_request, domain);

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int serve_mail_from(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (message_length == strlen("MAIL FROM: ")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* mail_from = get_value_from_buffer(buffer, ":");
	mail_from = trim_string(mail_from);

	if (is_empty_string(mail_from)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	if (!validate_email_string(mail_from)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	struct email_address* mail_from_email_address = string_to_email_address(mail_from);
	free(mail_from);
	smtp_request_set_mail_from(smtp_request, mail_from_email_address);

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int check_domain(char* domain, char* server_domain) {
	return !memcmp(domain, server_domain, strlen(server_domain));
}

static int check_user(char* user, char* server_user) {
	if (strlen(server_user) != strlen(user)) {
		return 0;
	}
	if (!memcmp(user, server_user, strlen(server_user))) {
		return 1;
	}
	return 0;
}

static int serve_rcpt_to(SOCKET sock, char* buffer, struct smtp_request* smtp_request, enum server_states current_state) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (!(current_state & HAS_FROM)) {
		status = send_response(sock, buffer, BAD_SEQUENCE);
		if (status == -1) return -1;
		return 0;
	}

	if (message_length == strlen("RCPT TO: ")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* rcpt_to = get_value_from_buffer(buffer, ":");
	rcpt_to = trim_string(rcpt_to);

	if (is_empty_string(rcpt_to)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	if (!validate_email_string(rcpt_to)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	struct email_address* rcpt_to_email_address = string_to_email_address(rcpt_to);
	free(rcpt_to);

	struct config config = config_parse_file("config.txt");

	char* server_domain = config_get_domain(&config);
	char* server_user = config_get_users(&config);

	if (!check_domain(rcpt_to_email_address->domain, server_domain)) {
		status = send_response(sock, buffer, USER_NOT_LOCAL);
		if (status == -1) return -1;
		return 0;
	}

	if (!check_user(rcpt_to_email_address->user, server_user)) {
		status = send_response(sock, buffer, USER_NOT_LOCAL);
		if (status == -1) return -1;
		return 0;
	}

	int added_recipient = smtp_request_add_recipient(smtp_request, rcpt_to_email_address);

	if (!added_recipient) {
		status = send_response(sock, buffer, TRANSACTION_FAILED);
		if (status == -1) return -1;
		return 0;
	}

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int delete_dot(char* buffer) {
	char* dot_pointer = strstr(buffer, "\r\n.\r\n", 5);
	if (dot_pointer != NULL) {
		*(dot_pointer + 2) = 0;
		return 1;
	}
	return 0;
}

static int end_of_message(char* buffer) {
	return strstr(buffer, "\r\n.\r\n", 5) != NULL;
}

static int get_smtp_line(char* message, char* buffer) {
	int minimum = MIN(strlen(buffer), BUFFER_SIZE);
	memcpy(message + strlen(message), buffer, minimum);
}

static char* get_smtp_data(SOCKET sock, char* buffer) {
	char* message = calloc(SMTP_REQUEST_MAIL_SIZE, sizeof(char));
	while (1) {
		int status = get_message(sock, buffer);
		if (status == -1) break;

		get_smtp_line(message, buffer);

		if (end_of_message(message)) break;
	}
	delete_dot(message);
	return message;
}

static int serve_data(SOCKET sock, char* buffer, struct smtp_request* smtp_request, enum server_states current_state) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (current_state != (HAS_FROM | HAS_TO)) {
		status = send_response(sock, buffer, BAD_SEQUENCE);
		if (status == -1) return -1;
		return 0;
	}

	if (message_length != strlen("DATA")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	status = send_response(sock, buffer, START_MAIL_INPUT);
	if (status == -1) return -1;

	char* smtp_data = get_smtp_data(sock, buffer);
	smtp_request_set_data(smtp_request, smtp_data);

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	deliver_mail(smtp_request);
	clean_smtp_request(smtp_request);
	smtp_request = init_smtp_request();

	return 1;
}

static int serve_rset(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (message_length != strlen("RSET")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	clean_smtp_request(smtp_request);
	smtp_request = init_smtp_request();

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;
	return 1;
}

static int serve_noop(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	int status = 0;
	int message_length = get_message_length(buffer);

	if (message_length != strlen("NOOP")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;
	return 1;
}

void serve_connection(SOCKET sock) {

	int status = 0;
	int current_state = DEFAULT;

	char* buffer = calloc(BUFFER_SIZE, sizeof(char));

	send_response(sock, buffer, SERVICE_READY);

	struct smtp_request* smtp_request = init_smtp_request();

	while (1) {

		status = get_message(sock, buffer);
		if (status == -1) {
			break;
		}

		if (buffer_has_command("QUIT", buffer)) {
			serve_quit(sock, buffer);
			break;
		}

		if (buffer_has_command("HELO ", buffer)) {
			status = serve_helo(sock, buffer, smtp_request);
			if (status == -1) break;
			continue;
		}

		if (buffer_has_command("MAIL FROM:", buffer)) {
			status = serve_mail_from(sock, buffer, smtp_request);
			if (status == -1) break;
			if (status == 1) current_state |= HAS_FROM;
			continue;
		}

		if (buffer_has_command("RCPT TO:", buffer)) {
			status = serve_rcpt_to(sock, buffer, smtp_request, current_state);
			if (status == -1) break;
			if (status == 1) current_state |= HAS_TO;
			continue;
		}

		if (buffer_has_command("DATA", buffer)) {
			status = serve_data(sock, buffer, smtp_request, current_state);
			if (status == -1) break;
			if (status == 1) current_state = DEFAULT;
			continue;
		}

		if (buffer_has_command("RSET", buffer)) {
			status = serve_rset(sock, buffer, smtp_request);
			if (status == -1) break;
			if (status == 1) current_state = DEFAULT;
			continue;
		}

		if (buffer_has_command("NOOP", buffer)) {
			status = serve_noop(sock, buffer, smtp_request);
			if (status == -1) break;
			continue;
		}

		status = send_response(sock, buffer, SYNTAX_ERROR);
		if (status == -1) break;
	}
	socket_cleanup(sock);
	free(buffer);
}