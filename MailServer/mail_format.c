#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mail.h"
#include "smtp_request.h"
#include "buffer.h"
#include "config.h"
#include "header.h"
#include "mail_format.h"

static enum STATUS add_single_header(struct mail* mail, char* header_line) {

	char* new_name = get_field_from_buffer(header_line, ":");
	new_name = trim_string(new_name);
	char* new_value = get_value_from_buffer(header_line, ":");
	new_value = trim_string(new_value);

	mail_add_header(mail, new_name, new_value);

	free(new_name);
	free(new_value);

	return STATUS_OK;
}

enum STATUS validate_header_line(char* line) {
	char* separator = strstr(line, ":");

	if (separator == NULL) return STATUS_NOT_OK;

	return STATUS_OK;
}

enum STATUS mail_parse_headers(struct mail* mail, char* mail_text) {
	char* iterator = mail_text;
	char* current_line;

	char* end_of_headers = strstr(mail_text, "\r\n\r\n");

	while (iterator != end_of_headers + strlen("\r\n")) {
		if (is_empty_string(iterator)) {
			mail_set_text(mail, iterator);
			return STATUS_OK;
		}

		current_line = buffer_get_next(&iterator, "\r\n");
		if (validate_header_line(current_line) == STATUS_NOT_OK) {
			mail_set_text(mail, iterator - strlen(current_line) - strlen("\r\n"));
			free(current_line);
			return STATUS_OK;
		}

		add_single_header(mail, current_line);
		free(current_line);
	}

	iterator = iterator + strlen("\r\n");
	mail_set_text(mail, iterator);

	return STATUS_OK;
}

static char* get_time_string() {
	time_t current_time = time(NULL);
	char* time_string = ctime(&current_time);

	*(time_string + strlen(time_string) - 1) = 0;

	return time_string;
}

enum STATUS mail_add_timestamp(struct mail* mail, struct email_address* mail_from) {
	char* mail_from_string = email_address_to_string(mail_from);

	char* time_string = get_time_string();
	char* timestamp = calloc(TIMESTAMP_SIZE, sizeof(char));

	flush_to_buffer(timestamp, TIMESTAMP_SIZE, "From %s %s\r\n", mail_from_string, time_string);

	free(mail_from_string);

	mail_set_timestamp(mail, timestamp);

	free(timestamp);

	return STATUS_OK;
}

static char* get_all_recipients(struct smtp_request* smtp_request) {
	char* result = calloc(DEFAULT_HEADER_SIZE, sizeof(char));

	struct smtp_request_recipient* current_recipient = smtp_request->rcpt_to_list;
	while (1) {
		char* recipient_user = email_address_to_string(current_recipient->email_address);
		if (current_recipient->list.prev == NULL) {
			add_to_buffer(result, recipient_user);
			free(recipient_user);
			break;
		}
		int recipient_user_length = strlen(recipient_user);
		flush_to_buffer(result, recipient_user_length + 3, "%s, ", recipient_user);
		current_recipient = list_parent(current_recipient->list.prev, struct smtp_request_recipient, list);
		free(recipient_user);
	}

	return result;
}

static char* get_received(char* hostname) {
	char* result = calloc(DEFAULT_HEADER_SIZE, sizeof(char));

	char* time_string = get_time_string();
	char* server_hostname = config_get_hostname();
	char* server_domain = config_get_domain();

	int hostname_length = strlen(hostname);
	int time_string_length = strlen(time_string);
	int server_domain_length = strlen(server_domain);
	int server_hostname_length = strlen(server_hostname);

	flush_to_buffer(result, hostname_length + time_string_length + server_hostname_length + server_domain_length + 14, "from %s by %s.%s; %s", hostname, server_hostname, server_domain, time_string);

	return result;
}

enum STATUS mail_add_server_headers(struct mail* mail, struct smtp_request* smtp_request) {
	char* mail_from_string = email_address_to_string(smtp_request->mail_from);
	mail_add_header_if_not_exists(mail, "From", mail_from_string);
	free(mail_from_string);

	char* all_recipients = get_all_recipients(smtp_request);
	mail_add_header_if_not_exists(mail, "To", all_recipients);
	mail_add_header(mail, "X-Original-To", all_recipients);
	free(all_recipients);

	char* received = get_received(smtp_request->hostname);
	mail_add_header(mail, "Received", received);
	free(received);

	return STATUS_OK;
}