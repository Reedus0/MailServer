#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include "message.h"
#include "codes.h"
#include "mail.h"
#include "delivery.h"
#include "server.h"
#include "email.h"

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
	add_to_message(response, code);
	return send_message(sock, response);
}

static int check_domain(char* domain, char* server_domain) {
	return !memcmp(domain, server_domain, strlen(server_domain));
}

static int check_user(char* user, char* server_users[]) {
	for (int i = 0; i < 2; i++) {
		char* current_user = server_users[i];
		if (strlen(current_user) != strlen(user)) {
			continue;
		}
		if (!memcmp(user, current_user, strlen(current_user))) {
			return 1;
		}
	}
	return 0;
}

static char* get_mail_content(SOCKET sock, char* buffer) {
	char* message = calloc(MAIL_SIZE, sizeof(char));
	while (1) {
		int status = get_message(sock, buffer);
		if (status == -1) break;

		if (!memcmp(buffer, ".\n", 2)) break;

		add_to_message(message, buffer);
	}
	return message;
}

static int serve_quit(SOCKET sock, char* buffer) {
	send_response(sock, buffer, SERVICE_CLOSING_TRANSMISSION);
}

static int serve_helo(SOCKET sock, char* buffer, struct raw_mail* raw_mail) {
	int status = 0;
	int message_length = strlen(buffer) - 1;

	if (message_length == strlen("HELO")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* domain = get_value_from_message(buffer, strlen("HELO"));
	domain = trim_string(domain);

	if (is_empty_string(domain)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	raw_mail->domain = domain;

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int serve_mail_from(SOCKET sock, char* buffer, struct raw_mail* raw_mail) {
	int status = 0;
	int message_length = strlen(buffer) - 1;

	if (message_length == strlen("MAIL FROM:")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* mail_from = get_value_from_message(buffer, strlen("MAIL FROM:"));
	mail_from = trim_string(mail_from);

	if (is_empty_string(mail_from)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	if (!validate_email(mail_from)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	raw_mail->mail_from = mail_from;

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int serve_rcpt_to(SOCKET sock, char* buffer, struct raw_mail* raw_mail, enum server_states current_state) {
	int status = 0;
	int message_length = strlen(buffer) - 1;

	char* server_domain = "domain";
	char* server_users[2] = {
		"john",
		"lol"
	};

	if (!(current_state & HAS_FROM)) {
		status = send_response(sock, buffer, BAD_SEQUENCE);
		if (status == -1) return -1;
		return 0;
	}

	if (message_length == strlen("RCPT TO:")) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* rcpt_to = get_value_from_message(buffer, strlen("RCPT TO:"));
	rcpt_to = trim_string(rcpt_to);

	if (is_empty_string(rcpt_to)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	if (!validate_email(rcpt_to)) {
		status = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		if (status == -1) return -1;
		return 0;
	}

	char* to_domain = get_email_domain(rcpt_to);

	if (!check_domain(to_domain, server_domain)) {
		status = send_response(sock, buffer, USER_NOT_LOCAL);
		if (status == -1) return -1;
		return 0;
	}

	free(to_domain);

	char* to_user = get_email_user(rcpt_to);

	if (!check_user(to_user, server_users)) {
		status = send_response(sock, buffer, USER_NOT_LOCAL);
		if (status == -1) return -1;
		return 0;
	}

	free(to_user);

	if (!raw_mail_add_recipient(raw_mail, rcpt_to)) {
		status = send_response(sock, buffer, TRANSACTION_FAILED);
		if (status == -1) return -1;
		return 0;
	}

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	return 1;
}

static int serve_data(SOCKET sock, char* buffer, struct raw_mail* raw_mail, enum server_states current_state) {
	int status = 0;
	int message_length = strlen(buffer) - 1;

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

	char* message = get_mail_content(sock, buffer);
	raw_mail->data = message;

	status = send_response(sock, buffer, ACTION_OK);
	if (status == -1) return -1;

	deliver_mail(*raw_mail);
	clean_raw_mail(raw_mail);

	return 1;
}

void serve_connection(SOCKET sock) {

	int status = 0;
	int current_state = DEFAULT;

	char* buffer = calloc(BUFFER_SIZE, sizeof(char));

	send_response(sock, buffer, SERVICE_READY);

	struct raw_mail raw_mail = init_raw_mail();

	while (1) {

		status = get_message(sock, buffer);
		if (status == -1) {
			break;
		}

		int message_length = strlen(buffer) - 1;

		if (check_message_command("QUIT", buffer)) {
			serve_quit(sock, buffer);
			break;
		}

		if (check_message_command("HELO", buffer)) {
			status = serve_helo(sock, buffer, &raw_mail);
			if (status == -1) break;
			continue;
		}

		if (check_message_command("MAIL FROM:", buffer)) {
			status = serve_mail_from(sock, buffer, &raw_mail);
			if (status == -1) break;
			current_state |= HAS_FROM;
			continue;
		}

		if (check_message_command("RCPT TO:", buffer)) {
			status = serve_rcpt_to(sock, buffer, &raw_mail, current_state);
			if (status == -1) break;
			current_state |= HAS_TO;
			continue;
		}

		if (check_message_command("DATA", buffer)) {
			status = serve_data(sock, buffer, &raw_mail, current_state);
			if (status == -1) break;
			current_state = DEFAULT;
			continue;
		}

		status = send_response(sock, buffer, SYNTAX_ERROR);
		if (status == -1) break;
	}
	socket_cleanup(sock);
	free(buffer);
}