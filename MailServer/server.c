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

static void clear_buffer(char* buffer) {
	memset(buffer, 0, BUFFER_SIZE);
}

static void socket_cleanup(SOCKET sock) {
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}

static int get_message(SOCKET sock, char* buffer) {
	clear_buffer(buffer);
	int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
	return bytes;
}

static int send_message(SOCKET sock, char* response) {
	int bytes = send(sock, response, strlen(response), 0);
	clear_buffer(response);
	return bytes;
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
		int bytes = get_message(sock, buffer);
		if (bytes == -1) break;

		if (!memcmp(buffer, ".\n", 2)) break;

		add_to_message(message, buffer);
	}
	return message;
}

void serve_connection(SOCKET sock) {

	char* server_domain = "domain";
	char* server_users[2] = {
		"john",
		"lol"
	};

	int bytes = 0;
	int current_state = DEFAULT;

	char* buffer = calloc(BUFFER_SIZE, sizeof(char));

	send_response(sock, buffer, SERVICE_READY);

	struct raw_mail new_mail = init_raw_mail();

	while (1) {

		bytes = get_message(sock, buffer);
		if (bytes == -1) {
			break;
		}

		if (check_message_command("QUIT", buffer)) {
			send_response(sock, buffer, SERVICE_CLOSING_TRANSMISSION);
			break;
		}

		if (check_message_command("HELO", buffer)) {
			if (bytes <= strlen("HELO") + 1) {
				bytes = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
				if (bytes == -1) break;
				continue;
			}

			char* domain = get_value_from_message(buffer, strlen("HELO"));
			new_mail.domain = domain;
			printf("%s", new_mail.domain);

			bytes = send_response(sock, buffer, ACTION_OK);
			if (bytes == -1) break;
			continue;
		}

		if (check_message_command("MAIL FROM:", buffer)) {
			if (bytes <= strlen("MAIL FROM:") + 1) {
				bytes = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
				if (bytes == -1) break;
				continue;
			}

			char* mail_from = get_value_from_message(buffer, strlen("MAIL FROM:"));

			if (!validate_email(mail_from)) {
				bytes = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
				if (bytes == -1) break;
				continue;
			}

			new_mail.mail_from = mail_from;
			printf("%s", new_mail.mail_from);

			bytes = send_response(sock, buffer, ACTION_OK);
			if (bytes == -1) break;

			current_state |= HAS_FROM;
			continue;
		}

		if (check_message_command("RCPT TO:", buffer)) {
			if (bytes <= strlen("RCPT TO:") + 1) {
				bytes = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
				if (bytes == -1) break;
				continue;
			}

			if (!(current_state & HAS_FROM)) {
				bytes = send_response(sock, buffer, BAD_SEQUENCE);
				if (bytes == -1) break;
				continue;
			}

			char* rcpt_to = get_value_from_message(buffer, strlen("RCPT TO:"));

			if (!validate_email(rcpt_to)) {
				bytes = send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
				if (bytes == -1) break;
				continue;
			}

			char* to_domain = get_email_domain(rcpt_to);
			char* to_user = get_email_user(rcpt_to);

			if (!check_domain(to_domain, server_domain)) {
				bytes = send_response(sock, buffer, USER_NOT_LOCAL);
				if (bytes == -1) break;
				continue;
			}

			if (!check_user(to_user, server_users)) {
				bytes = send_response(sock, buffer, USER_NOT_LOCAL);
				if (bytes == -1) break;
				continue;
			}

			free(to_domain);
			free(to_user);

			if (new_mail.rcpt_count >= RECIPIENT_COUNT) {
				bytes = send_response(sock, buffer, TRANSACTION_FAILED);
				if (bytes == -1) break;
				continue;
			}
			new_mail.rcpt_to_arr[new_mail.rcpt_count] = rcpt_to;
			new_mail.rcpt_count += 1;
			printf("%s", new_mail.rcpt_to_arr[new_mail.rcpt_count - 1]);

			bytes = send_response(sock, buffer, ACTION_OK);
			if (bytes == -1) break;

			current_state |= HAS_TO;
			continue;
		}

		if (check_message_command("DATA", buffer)) {
			if (current_state != (HAS_FROM | HAS_TO)) {
				bytes = send_response(sock, buffer, BAD_SEQUENCE);
				if (bytes == -1) break;
				continue;
			}

			bytes = send_response(sock, buffer, START_MAIL_INPUT);
			if (bytes == -1) break;

			char* message = get_mail_content(sock, buffer);
			new_mail.data = message;

			printf("%s", new_mail.data);

			bytes = send_response(sock, buffer, ACTION_OK);
			if (bytes == -1) break;

			deliver_mail(new_mail);
			new_mail = clean_raw_mail(new_mail);

			current_state = DEFAULT;
			continue;
		}

		bytes = send_response(sock, buffer, SYNTAX_ERROR);
		if (bytes == -1) break;
	}
	socket_cleanup(sock);
	free(buffer);
}