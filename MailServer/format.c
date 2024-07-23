#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "message.h"

void parse_headers(struct mail mail, char* mail_text) {
	char* text_ptr = mail_text;
	char* temp_ptr = mail_text;
	while (1) {
		if (*text_ptr == '\n') {
			char* separator = strchr(temp_ptr, ':');
			if (separator == NULL) {
				break;
			}
			int name_length = separator - temp_ptr;
			int value_length = text_ptr - separator - 2;
			mail.headers[mail.headers_count].name = calloc(name_length, sizeof(char));
			memcpy(mail.headers[mail.headers_count].name, temp_ptr, name_length);
			*(mail.headers[mail.headers_count].name + name_length) = 0;
			mail.headers[mail.headers_count].value = calloc(value_length, sizeof(char));
			memcpy(mail.headers[mail.headers_count].value, separator + 2, value_length);
			*(mail.headers[mail.headers_count].value + value_length) = 0;
			temp_ptr = text_ptr + 1;
			mail.headers_count += 1;
		}
		text_ptr += 1;
	}
}

void add_header(struct mail mail, char* name, char* value) {
	mail.headers[mail.headers_count].name = calloc(strlen(name), sizeof(char));
	memcpy(mail.headers[mail.headers_count].name, name, strlen(name));
	*(mail.headers[mail.headers_count].name + strlen(name)) = 0;
	mail.headers[mail.headers_count].value = calloc(strlen(value), sizeof(char));
	memcpy(mail.headers[mail.headers_count].value, value, strlen(value));
	*(mail.headers[mail.headers_count].value + strlen(value)) = 0;
	mail.headers_count += 1;
}

void get_mail_text(struct mail mail, char* mail_text) {
	mail.text = calloc(MAIL_SIZE, sizeof(char));
	char* text_ptr = mail_text;
	while (1) {
		if (!memcmp(text_ptr, "\n\n", 2)) {
			text_ptr += 2;
			memcpy(mail.text, text_ptr, strlen(text_ptr));
			break;
		}
		text_ptr += 1;
	}
}