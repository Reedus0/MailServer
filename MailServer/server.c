#include <stdio.h>
#include <Winsock2.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include "message.h"
#include "codes.h"
#include "mail.h"

static int get_message(SOCKET sock, char* buffer) {
	return recv(sock, buffer, BUFFER_SIZE, 0);
}

static int send_response(SOCKET sock, char* response) {
	return send(sock, response, strlen(response), 0);
}

static void clear_buffer(char* buffer) {
	memset(buffer, 0, BUFFER_SIZE);
}

void serve_connection(SOCKET sock) {
	int status = 0;
	char* buffer = calloc(BUFFER_SIZE, sizeof(char));

	build_message(buffer, SERVICE_READY, " Hello, from server!", "\n");
	send_response(sock, buffer);
	clear_buffer(buffer);

	struct mail new_mail;

	while (1) {
		status = get_message(sock, buffer);
		if (status == -1) {
			break;
		}

		if (!memcmp(buffer, "HELO", 4)) {
			int new_length = strlen(buffer) - 5;
			char* domain = calloc(new_length, sizeof(char));
			memcpy(domain, buffer + 5, new_length - 1);
			*(domain + new_length) = 0;
			new_mail.domain = domain;
			printf("%s", new_mail.domain);
			continue;
		}

		if (!memcmp(buffer, "MAIL FROM:", 10)) {
			int new_length = strlen(buffer) - 11;
			char* mail_from = calloc(new_length, sizeof(char));
			memcpy(mail_from, buffer + 11, new_length - 1);
			*(mail_from + new_length) = 0;
			new_mail.mail_from = mail_from;
			printf("%s", new_mail.mail_from);
			continue;
		}

		if (!memcmp(buffer, "RCPT TO:", 8)) {
			int new_length = strlen(buffer) - 9;
			char* rcpt_to = calloc(new_length, sizeof(char));
			memcpy(rcpt_to, buffer + 9, new_length - 1);
			*(rcpt_to + new_length) = 0;
			new_mail.rcpt_to = rcpt_to;
			printf("%s", new_mail.rcpt_to);
			continue;
		}

		if (!memcmp(buffer, "DATA", 4)) {
			clear_buffer(buffer);
			build_message(buffer, START_MAIL_INPUT, " End mail with single .", "\n");
			status = send_response(sock, buffer);
			if (status == -1) {
				break;
			}
			clear_buffer(buffer);

			char* message = calloc(MAIL_SIZE, sizeof(char));
			while (1) {
				status = get_message(sock, buffer);
				if (status == -1) {
					break;
				}

				if (!memcmp(buffer, ".\n", 2)) {
					break;
				}

				add_to_message(message, buffer);
				clear_buffer(buffer);

			}
			int new_length = strlen(message);
			*(message + new_length) = 0;
			new_mail.data = message;

			printf("%s", new_mail.data);

			clear_buffer(buffer);
			build_message(buffer, ACTION_OK, " Got mail", "\n");
			status = send_response(sock, buffer);
			if (status == -1) {
				break;
			}
			clear_buffer(buffer);
			continue;
		}

		if (!memcmp(buffer, "QUIT", 4)) {
			build_message(buffer, SERVICE_CLOSING_TRANSMISSION, " Bye", "\n");
			status = send_response(sock, buffer);
			if (status == -1) {
				break;
			}
			clear_buffer(buffer);
			break;
		}

		clear_buffer(buffer);
		build_message(buffer, SYNTAX_ERROR, " Syntex error", "\n");
		status = send_response(sock, buffer);
		if (status == -1) {
			break;
		}
		clear_buffer(buffer);
	}
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}