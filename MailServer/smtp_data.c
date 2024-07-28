#include <string.h>
#include <WinSock2.h>
#include "smtp_data.h"
#include "codes.h"
#include "header.h"
#include "server.h"
#include "net.h"
#include "buffer.h"

static enum STATUS delete_dots(char* buffer) {
	char* dot_pointer = strstr(buffer, "\r\n.", 3);
	while (dot_pointer != NULL) {
		*(dot_pointer + strlen("\r\n")) = 0;
		dot_pointer = dot_pointer + strlen("\r\n") + 1;
		add_to_buffer(buffer, dot_pointer);
		dot_pointer = strstr(dot_pointer, "\r\n.", 3);
	}
	return STATUS_OK;
}

static int smtp_data_ended(char* buffer) {
	return strstr(buffer, "\r\n.\r\n", 5) != NULL;
}

static enum STATUS get_smtp_data_line(char* message, char* buffer) {
	int minimum = MIN(strlen(buffer), BUFFER_SIZE);
	memcpy(message + strlen(message), buffer, minimum);
	return STATUS_OK;
}

static char* get_smtp_data(SOCKET sock, char* buffer) {
	char* message = calloc(SMTP_REQUEST_MAIL_SIZE, sizeof(char));
	while (1) {
		int status = get_message(sock, buffer);
		if (status == STATUS_ERROR) break;

		get_smtp_data_line(message, buffer);

		if (smtp_data_ended(message)) break;
	}
	delete_dots(message);
	return message;
}

enum STATUS serve_data(SOCKET sock, char* buffer, struct smtp_request* smtp_request, enum server_states current_state) {
	if (validate_without_args(buffer, "DATA") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	send_response(sock, buffer, START_MAIL_INPUT);

	char* smtp_data = get_smtp_data(sock, buffer);
	smtp_request_set_data(smtp_request, smtp_data);

	send_response(sock, buffer, ACTION_OK);

	deliver_mail(smtp_request);
	clean_smtp_request(smtp_request);

	return STATUS_OK;
}