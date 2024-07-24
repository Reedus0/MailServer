#include <stdio.h>
#include <malloc.h>
#include "mail.h"

struct raw_mail init_raw_mail() {
	struct raw_mail new_raw_mail;
	new_raw_mail.domain = NULL;
	new_raw_mail.mail_from = NULL;
	new_raw_mail.data = NULL;
	new_raw_mail.rcpt_count = 0;

	for (int i = 0; i < RECIPIENT_COUNT; i++) {
		char* current_recipient = new_raw_mail.rcpt_to_arr[i];
		current_recipient = NULL;
	}

	return new_raw_mail;
}

int clean_raw_mail(struct raw_mail* raw_mail) {
	free(raw_mail->domain);
	free(raw_mail->mail_from);
	free(raw_mail->data);

	for (int i = 0; i < raw_mail->rcpt_count; i++) {
		char* current_recipient = raw_mail->rcpt_to_arr[i];
		free(current_recipient);
		current_recipient = NULL;
	}

	raw_mail->domain = NULL;
	raw_mail->mail_from = NULL;
	raw_mail->data = NULL;
	raw_mail->rcpt_count = 0;
	return 1;
}

int raw_mail_add_recipient(struct raw_mail* raw_mail, char* recipient) {
	if (raw_mail->rcpt_count >= RECIPIENT_COUNT) return 0;

	raw_mail->rcpt_to_arr[raw_mail->rcpt_count] = recipient;
	raw_mail->rcpt_count += 1;
	return 1;
}

struct mail init_mail() {
	struct mail new_mail;
	new_mail.text = NULL;
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

char* build_mail(struct mail* mail) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
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

	for (int i = 0; i < mail->headers_count; i++) {
		struct mail_header* current_header = &mail->headers[i];
		free(current_header->name);
		free(current_header->value);
		current_header->name = NULL;
		current_header->value = NULL;
	}

	mail->text = NULL;
	mail->headers_count = 0;
	return 1;
}