#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mail.h"
#include "smtp_request.h"
#include "buffer.h"
#include "config.h"

static int add_single_header(struct mail* mail, char* header_line) {

	char* new_name = get_field_from_buffer(header_line, ": ");
	new_name = trim_string(new_name);
	char* new_value = get_value_from_buffer(header_line, ": ");

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

	int header_line_length = end_of_line - mait_text + 1;

	char* header_line = calloc(header_line_length, sizeof(char));
	memcpy(header_line, mait_text, header_line_length - 1);

	return header_line;
}

static int mail_parse_headers(struct mail* mail, char* mail_text) {
	char* char_pointer = strstr(mail_text, "\r\n");
	char* end_of_headers = strstr(mail_text, "\r\n\r\n");
	char* base_line = mail_text;
	while (base_line != end_of_headers + 2) {
		char* current_line = get_header_line(base_line);

		if (current_line == NULL || *current_line == NULL) {
			char* new_text = copy_buffer(base_line);
			mail_set_text(mail, new_text);
			return 0;
		}

		if (!add_single_header(mail, current_line)) {
			return 0;
		}
		
		free(current_line);

		base_line = char_pointer + 2;
		char_pointer = strstr(base_line, "\r\n");
	}

	char* new_text = copy_buffer(end_of_headers + 4);
	mail_set_text(mail, new_text);

	return 1;
}

static char* get_time_string() {
	time_t current_time = time(NULL);
	char* time_string = ctime(&current_time);

	*(time_string + strlen(time_string) - 1) = 0;

	return time_string;
}

static int mail_get_timestamp(struct mail* mail, struct email_address* mail_from) {
	char* mail_from_string = email_address_to_string(mail_from);

	char* time_string = get_time_string();
	char* timestamp = calloc(TIMESTAMP_SIZE, sizeof(char));

	flush_to_buffer(timestamp, TIMESTAMP_SIZE, "From %s %s\r\n", mail_from_string, time_string);

	free(mail_from_string);

	mail_set_timestamp(mail, timestamp);

	return 1;
}

int format_mail(struct mail* mail, struct smtp_request* smtp_request) {
	mail_get_timestamp(mail, smtp_request->mail_from);
	mail_parse_headers(mail, smtp_request->data);

	return 1;
}