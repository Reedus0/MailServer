#include <stdio.h>
#include <malloc.h>
#include "mail.h"
#include "buffer.h"

static struct mail_header* init_mail_header() {
	struct mail_header* new_mail_header = calloc(1, sizeof(struct mail_header));
	new_mail_header->name = NULL;
	new_mail_header->value = NULL;

	return new_mail_header;
}

static int clean_mail_header(struct mail_header* mail_header) {
	free(mail_header->name);
	free(mail_header->value);

	free(mail_header);
	return 1;
}

struct mail* init_mail() {
	struct mail* new_mail = calloc(1, sizeof(struct mail));
	new_mail->text = NULL;
	new_mail->timestamp = NULL;
	new_mail->headers_count = 0;
	
	for (int i = 0; i < HEADERS_COUNT; i++) {
		new_mail->headers[i] = NULL;
	}

	return new_mail;
}

int mail_add_header(struct mail* mail, char* name, char* value) {
	if (mail->headers_count >= HEADERS_COUNT) return 0;

	struct mail_header* new_mail_header = init_mail_header();

	new_mail_header->name = name;
	new_mail_header->value = value;

	mail->headers[mail->headers_count] = new_mail_header;
	mail->headers_count += 1;

	return 1;
}

int mail_set_text(struct mail* mail, char* text) {
	mail->text = text;
	return 1;
}

int mail_set_timestamp(struct mail* mail, char* timestamp) {
	mail->timestamp = timestamp;
	return 1;
}

char* build_mail(struct mail* mail) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
	add_to_buffer(result, mail->timestamp);
	for (int i = 0; i < mail->headers_count; i++) {
		struct mail_header* current_header = mail->headers[i];
		int name_length = strlen(current_header->name);
		int value_length = strlen(current_header->value) + 1;
		flush_to_buffer(result, name_length + value_length + 3, "%s: %s\n", current_header->name, current_header->value);
	}

	int text_length = strlen(mail->text) + 1;
	flush_to_buffer(result, text_length + 2, "\n%s\n", mail->text);

	return result;
}

int clean_mail(struct mail* mail) {
	free(mail->text);
	free(mail->timestamp);

	for (int i = 0; i < mail->headers_count; i++) {
		struct mail_header* current_header = mail->headers[i];
		clean_mail_header(current_header);
	}

	free(mail);
	return 1;
}