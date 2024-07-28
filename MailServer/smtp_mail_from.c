#include <string.h>
#include <WinSock2.h>
#include "smtp_data.h"
#include "header.h"
#include "codes.h"
#include "net.h"
#include "server.h"
#include "buffer.h"
#include "smtp_request.h"

enum STATUS serve_mail_from(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	if (validate_with_args(buffer, "MAIL FROM:", ":") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	char* mail_from = get_value_from_buffer(buffer, ":");
	mail_from = trim_string(mail_from);

	if (!validate_email_string(mail_from)) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	struct email_address* mail_from_email_address = string_to_email_address(mail_from);
	free(mail_from);
	smtp_request_set_mail_from(smtp_request, mail_from_email_address);

	send_response(sock, buffer, ACTION_OK);

	return STATUS_OK;
}