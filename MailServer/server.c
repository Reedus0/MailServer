#include <stdio.h>
#include <Winsock2.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include "message.h"
#include "codes.h"
#include "mail.h"

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

static int send_response(SOCKET sock, char* response) {
	int status = send(sock, response, strlen(response), 0);
	clear_buffer(response);
	return status;
}

static int send_code_response(SOCKET sock, char* response, char* code) {
	clear_buffer(response);
	add_to_message(response, code);
	return send_response(sock, response);
}

static char* get_value_from_message(char* message, int offset) {
	int new_length = strlen(message) - offset + 1;
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, message + offset + 1, new_length);
	*(result + strlen(result)) = 0;
	return result;
}

static int check_message_command(char* command, char* message) {
	return !memcmp(message, command, strlen(command));
}

static char* get_mail_content(SOCKET sock, char* buffer) {
	char* message = calloc(MESSAGE_SIZE, sizeof(char));
	while (1) {
		int status = get_message(sock, buffer);
		if (status == -1) break;

		if (!memcmp(buffer, ".\n", 2)) break;

		add_to_message(message, buffer);
	}
	return message;
}

void serve_connection(SOCKET sock) {
	int status = 0;
	char* buffer = calloc(BUFFER_SIZE, sizeof(char));

	send_code_response(sock, buffer, SERVICE_READY);

	struct mail new_mail;

	while (1) {
		status = get_message(sock, buffer);
		if (status == -1) {
			break;
		}

		if (check_message_command("QUIT", buffer)) {
			send_code_response(sock, buffer, SERVICE_CLOSING_TRANSMISSION);
			break;
		}

		if (check_message_command("HELO", buffer)) {
			char* domain = get_value_from_message(buffer, strlen("HELO"));
			new_mail.domain = domain;
			printf("%s", new_mail.domain);

			status = send_code_response(sock, buffer, ACTION_OK);
			if (status == -1) break;
			continue;
		}

		if (check_message_command("MAIL FROM:", buffer)) {
			char* mail_from = get_value_from_message(buffer, strlen("MAIL FROM:"));
			new_mail.mail_from = mail_from;
			printf("%s", new_mail.mail_from);

			status = send_code_response(sock, buffer, ACTION_OK);
			if (status == -1) break;
			continue;
		}

		if (check_message_command("RCPT TO:", buffer)) {
			char* rcpt_to = get_value_from_message(buffer, strlen("RCPT TO:"));
			new_mail.rcpt_to = rcpt_to;
			printf("%s", new_mail.rcpt_to);

			status = send_code_response(sock, buffer, ACTION_OK);
			if (status == -1) break;
			continue;
		}

		if (check_message_command("DATA", buffer)) {
			status = send_code_response(sock, buffer, START_MAIL_INPUT);
			if (status == -1) break;

			char* message = get_mail_content(sock, buffer);
			new_mail.data = message;

			printf("%s", new_mail.data);

			status = send_code_response(sock, buffer, ACTION_OK);
			if (status == -1) break;
			continue;
		}

		status = send_code_response(sock, buffer, SYNTAX_ERROR);
		if (status == -1) break;
	}
	socket_cleanup(sock);
}