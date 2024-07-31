#include <string.h>
#include <WinSock2.h>
#include "smtp_data.h"
#include "email_address.h"
#include "codes.h"
#include "header.h"
#include "server.h"
#include "net.h"
#include "buffer.h"
#include "smtp_request.h"
#include "config.h"
#include "validation.h"

static enum STATUS check_domain(char* domain) {
	char* server_domain = config_get_domain();
	return !memcmp(domain, server_domain, strlen(server_domain));
}

static enum STATUS check_user(char* user) {
	struct user* current_user = config_get_users();
	while (1) {
		if (compare_strings(current_user->username, user)) {
			return STATUS_OK;
		}
		if (current_user->list.prev == NULL) {
			break;
		}
		current_user = list_parent(current_user->list.prev, struct user, list);
	}
	return STATUS_NOT_OK;
}

static enum STATUS validate_user(struct email_address* user) {

	if (check_domain(user->domain) == STATUS_NOT_OK) {
		return STATUS_NOT_OK;
	}

	if (check_user(user->user) == STATUS_NOT_OK) {
		return STATUS_NOT_OK;
	}

	return STATUS_OK;
}

enum STATUS serve_rcpt_to(SOCKET sock, char* buffer, struct smtp_request* smtp_request) {
	if (validate_with_args(buffer, "rcpt to:", ":") == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	char* rcpt_to = get_value_from_buffer(buffer, ":");
	rcpt_to = trim_string(rcpt_to);

	if (validate_email_string(rcpt_to) == STATUS_NOT_OK) {
		send_response(sock, buffer, SYNTAX_ERROR_PARAMETERS);
		return STATUS_NOT_OK;
	}

	struct email_address* rcpt_to_email_address = string_to_email_address(rcpt_to);
	free(rcpt_to);

	if (validate_user(rcpt_to_email_address) == STATUS_NOT_OK) {
		send_response(sock, buffer, USER_NOT_LOCAL);
		clean_email_address(rcpt_to_email_address);
		return STATUS_NOT_OK;
	}

	smtp_request_add_recipient(smtp_request, rcpt_to_email_address);

	send_response(sock, buffer, ACTION_OK);

	return STATUS_OK;
}