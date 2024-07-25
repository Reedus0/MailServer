#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mail.h"
#include "smtp_request.h"
#include "buffer.h"
#include "config.h"

static int add_single_header(struct mail* mail, char* header_line) {

	char* separator = strchr(header_line, ':');

	char* new_name = get_field_from_buffer(header_line, (separator - header_line));
	char* new_value = get_value_from_buffer(header_line, (separator - header_line) + 1);

	return mail_add_header(mail, new_name, new_value);
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

		if (!add_single_header(mail, current_line)) {
			return 0;
		}
		
		free(current_line);

		base_line = char_pointer + 1;
		char_pointer = strchr(base_line, '\n');
	}

	return 1;
}

static int mail_get_text(struct mail* mail, char* mail_text) {

	if (mail->headers_count == 0) {
		char* new_text = copy_buffer(mail_text);
		mail_set_text(mail, new_text);
		return 1;
	}

	char* text_pointer = strstr(mail_text, "\n\n");
	char* new_text = copy_buffer(text_pointer + 2);
	mail_set_text(mail, new_text);
	return 1;
}

static char* get_time_string() {
	time_t current_time = time(NULL);
	char* time_string = ctime(&current_time);

	*(time_string + strlen(time_string) - 1) = 0;

	return time_string;
}

static char* get_user_domain(char* domain) {
	char* user_domain;

	if (domain == NULL) {
		char* unknown_domain = "UNKNOWN";
		user_domain = calloc(strlen(unknown_domain) + 1, sizeof(char));
		memcpy(user_domain, unknown_domain, strlen(unknown_domain));
	}
	else {
		user_domain = calloc(strlen(domain) + 1, sizeof(char));
		memcpy(user_domain, domain, strlen(domain));
	}

	return user_domain;
}

static int mail_get_timestamp(struct mail* mail, char* domain) {
	struct config config = config_parse_file("config.txt");

	char* user_domain = get_user_domain(domain);
	char* server_domain = config_get_domain(&config);
	char* server_hostname = config_get_hostname(&config);

	char* time_string = get_time_string();
	char* timestamp = calloc(TIMESTAMP_SIZE, sizeof(char));

	flush_to_buffer(timestamp, TIMESTAMP_SIZE, "Recieved: from %s by %s.%s; %s\n", user_domain, server_hostname, server_domain, time_string);

	mail_set_timestamp(mail, timestamp);

	return 1;
}

int format_mail(struct mail* mail, struct smtp_request* smtp_request) {
	mail_get_timestamp(mail, smtp_request->domain);
	mail_parse_headers(mail, smtp_request->data);
	mail_get_text(mail, smtp_request->data);

	return 1;
}