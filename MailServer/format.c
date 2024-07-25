#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "smtp_request.h"
#include "message.h"

static int parse_single_header(struct mail* mail, char* header_line) {

	char* separator = strchr(header_line, ':');

	char* new_name = get_field_from_message(header_line, (separator - header_line));
	char* new_value = get_value_from_message(header_line, (separator - header_line) + 1);

	mail_add_header(mail, new_name, new_value);
}

static char* get_header_line(char* mait_text) {
	char* end_of_line = strchr(mait_text, '\n') + 1;
	char* separator = strchr(mait_text, ':');

	if (separator == NULL) {
		return NULL;
	}

	int header_line_length = end_of_line - mait_text + 1;

	char* header_line = calloc(header_line_length, sizeof(char));
	memcpy(header_line, mait_text, header_line_length - 1);

	return header_line;
}

static int mail_parse_headers(struct mail* mail, char* mail_text) {
	char* char_pointer = strchr(mail_text, '\n');
	char* end_of_headers = strstr(mail_text, "\n\n");
	char* base_line = mail_text;
	while (base_line != end_of_headers + 1) {
		char* current_line = get_header_line(base_line);
		if (current_line == NULL) {
			return 0;
		}
		parse_single_header(mail, current_line);
		free(current_line);

		base_line = char_pointer + 1;
		char_pointer = strchr(base_line, '\n');
	}

	return 1;
}

static int mail_get_text(struct mail* mail, char* mail_text) {
	
	if (mail->headers_count == 0) {
		mail_set_text(mail, mail_text);
		return 1;
	}

	char* text_pointer = strstr(mail_text, "\n\n");
	mail_set_text(mail, text_pointer + 2);
	return 1;
}

int format_mail(struct mail* mail, struct smtp_request* smtp_request) {
	mail_parse_headers(mail, smtp_request->data);
	mail_get_text(mail, smtp_request->data);

	return 1;
}