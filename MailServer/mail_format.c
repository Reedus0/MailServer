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

static int add_single_header(struct mail* mail, char* header_line) {

	char* new_name = get_field_from_buffer(header_line, ":");
	new_name = trim_string(new_name);
	char* new_value = get_value_from_buffer(header_line, ":");
	new_value = trim_string(new_value);

	return mail_add_header(mail, new_name, new_value);
}

static char* get_header_line(char* mait_text) {
	char* start_of_line = mait_text;
	char* end_of_line = strstr(mait_text, "\r\n");
	char* separator = strchr(mait_text, ':');

	if (*start_of_line == ' ') {
		return NULL;
	}

	if (separator == NULL) {
		return NULL;
	}

	if (separator > end_of_line) {
		return NULL;
	}

	int header_line_length = end_of_line - mait_text;

	char* header_line = calloc(header_line_length + 1, sizeof(char));
	memcpy(header_line, mait_text, header_line_length);

	return header_line;
}

static enum STATUS mail_parse_headers(struct mail* mail, char* mail_text) {
	char* char_pointer = strstr(mail_text, "\r\n");
	char* end_of_headers = strstr(mail_text, "\r\n\r\n");
	char* base_line = mail_text;
	while (base_line != end_of_headers + strlen("\r\n")) {
		char* current_line = get_header_line(base_line);

		if (current_line == NULL || *current_line == NULL) {
			char* new_text = copy_buffer(base_line);
			mail_set_text(mail, new_text);
			return STATUS_NOT_OK;
		}

		if (!add_single_header(mail, current_line)) {
			return STATUS_NOT_OK;
		}
		
		free(current_line);

		base_line = char_pointer + strlen("\r\n");
		char_pointer = strstr(base_line, "\r\n");
	}

	char* new_text = copy_buffer(end_of_headers + strlen("\r\n\r\n"));
	mail_set_text(mail, new_text);

	return STATUS_OK;
}

static char* get_time_string() {
	time_t current_time = time(NULL);
	char* time_string = ctime(&current_time);

	*(time_string + strlen(time_string) - 1) = 0;

	return time_string;
}

static enum STATUS mail_add_timestamp(struct mail* mail, struct email_address* mail_from) {
	char* mail_from_string = email_address_to_string(mail_from);

	char* time_string = get_time_string();
	char* timestamp = calloc(TIMESTAMP_SIZE, sizeof(char));

	flush_to_buffer(timestamp, TIMESTAMP_SIZE, "From %s %s\r\n", mail_from_string, time_string);

	free(mail_from_string);

	mail_set_timestamp(mail, timestamp);

	return STATUS_OK;
}

static char* get_all_recipients(struct smtp_request* smtp_request) {
	char* result = calloc(DEFAULT_HEADER_SIZE, sizeof(char));

	struct smtp_request_recipient* current_recipient = smtp_request->rcpt_to_list;
	while (1) {
		char* recipient_user = email_address_to_string(current_recipient->email_address);
		if (current_recipient->list.prev == NULL) {
			add_to_buffer(result, recipient_user);
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

static enum STATUS mail_add_server_headers(struct mail* mail, struct smtp_request* smtp_request) {
	mail_add_header_if_not_exists(mail, "From", email_address_to_string(smtp_request->mail_from));
	char* all_recipients = get_all_recipients(smtp_request);
	mail_add_header_if_not_exists(mail, "To", all_recipients);
	mail_add_header(mail, "X-Original-To", all_recipients);
	char* received = get_received(smtp_request->hostname);
	mail_add_header(mail, "Received", received);
	return STATUS_OK;
}

int format_mail(struct mail* mail, struct smtp_request* smtp_request) {
	mail_parse_headers(mail, smtp_request->data);
	mail_add_timestamp(mail, smtp_request->mail_from);
	//mail_add_server_headers(mail, smtp_request);

	return STATUS_OK;
}