#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "message.h"

struct mail parse_headers(struct mail mail, char* mail_text) {
	char* char_pointer = mail_text;
	char* header_line = mail_text;
	while (1) {
		if (*char_pointer == '\n') {
			char* separator = strchr(header_line, ':');
			if (separator == NULL) {
				break;
			}

			char* current_line = calloc(char_pointer - header_line + 2, sizeof(char));
			memcpy(current_line, header_line, char_pointer - header_line + 1);
			char* new_name = get_field_from_message(current_line, (separator - header_line));
			mail.headers[mail.headers_count].name = new_name;

			char* new_value = get_value_from_message(current_line, (separator - header_line) + 1);
			mail.headers[mail.headers_count].value = new_value;
			header_line = char_pointer + 1;
			mail.headers_count += 1;

			free(current_line);
		}
		char_pointer += 1;
	}

	return mail;
}

struct mail add_header(struct mail mail, char* name, char* value) {

	char* new_name = calloc(strlen(name) + 1, sizeof(char));
	memcpy(new_name, name, strlen(name));
	mail.headers[mail.headers_count].name = new_name;

	char* new_value = calloc(strlen(value) + 1, sizeof(char));
	memcpy(new_value, value, strlen(value));
	mail.headers[mail.headers_count].value = new_value;

	mail.headers_count += 1;
	return mail;
}

struct mail get_mail_text(struct mail mail, char* mail_text) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
	char* char_pointer = mail_text;
	char* text_pointer = mail_text;
	while (1) {
		if (*char_pointer == '\n') {
			char* separator = strchr(text_pointer, ':');
			if (separator == NULL) {
				break;
			}
			text_pointer = char_pointer + 1;
		}
		char_pointer += 1;
	}
	if (*text_pointer == '\n') {
		text_pointer += 1;
	}
	memcpy(result, text_pointer, strlen(text_pointer));
	mail.text = result;
	return mail;
}

char* build_mail(struct mail mail) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
	for (int i = 0; i < mail.headers_count; i++) {
		struct mail_header current_header = mail.headers[i];
		add_to_message(result, current_header.name);
		add_to_message(result, ": ");
		add_to_message(result, current_header.value);
		add_to_message(result, "\n");
	}

	add_to_message(result, "\n");
	add_to_message(result, mail.text);
	add_to_message(result, "\n");

	return result;
}