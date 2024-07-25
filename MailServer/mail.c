#include <stdio.h>
#include <malloc.h>
#include "mail.h"

struct mail init_mail() {
	struct mail new_mail;
	new_mail.text = NULL;
	new_mail.timestamp = NULL;
	new_mail.headers_count = 0;
	
	for (int i = 0; i < HEADERS_COUNT; i++) {
		struct mail_header new_header;
		new_header.name = NULL;
		new_header.value = NULL;
		new_mail.headers[i] = new_header;
	}

	return new_mail;
}

int mail_add_header(struct mail* mail, char* name, char* value) {
	if (mail->headers_count >= HEADERS_COUNT) return 0;

	char* new_name = calloc(strlen(name) + 1, sizeof(char));
	memcpy(new_name, name, strlen(name));
	mail->headers[mail->headers_count].name = new_name;

	char* new_value = calloc(strlen(value) + 1, sizeof(char));
	memcpy(new_value, value, strlen(value));
	mail->headers[mail->headers_count].value = new_value;

	mail->headers_count += 1;
	return 1;
}

int mail_set_text(struct mail* mail, char* text) {
	char* mail_text = calloc(MAIL_SIZE, sizeof(char));
	memcpy(mail_text, text, strlen(text));
	mail->text = mail_text;
	return 1;
}

int mail_set_timestamp(struct mail* mail, char* timestamp) {
	char* mail_timestamp = calloc(strlen(timestamp) + 1, sizeof(char));
	memcpy(mail_timestamp, timestamp, strlen(timestamp));
	mail->timestamp = mail_timestamp;
	return 1;
}

char* build_mail(struct mail* mail) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
	add_to_message(result, mail->timestamp);
	for (int i = 0; i < mail->headers_count; i++) {
		struct mail_header current_header = mail->headers[i];
		add_to_message(result, current_header.name);
		add_to_message(result, ": ");
		add_to_message(result, current_header.value);
		add_to_message(result, "\n");
	}

	add_to_message(result, "\n");
	add_to_message(result, mail->text);
	add_to_message(result, "\n");

	return result;
}

int clean_mail(struct mail* mail) {
	free(mail->text);
	free(mail->timestamp);

	for (int i = 0; i < mail->headers_count; i++) {
		struct mail_header* current_header = &mail->headers[i];
		free(current_header->name);
		free(current_header->value);
		current_header->name = NULL;
		current_header->value = NULL;
	}

	mail->text = NULL;
	mail->timestamp = NULL;
	mail->headers_count = 0;
	return 1;
}