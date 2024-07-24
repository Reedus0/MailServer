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

struct raw_mail clean_raw_mail(struct raw_mail raw_mail) {
	free(raw_mail.domain);
	free(raw_mail.mail_from);
	free(raw_mail.data);

	for (int i = 0; i < raw_mail.rcpt_count; i++) {
		char* current_recipient = raw_mail.rcpt_to_arr[i];
		free(current_recipient);
		current_recipient = NULL;
	}

	raw_mail.domain = NULL;
	raw_mail.mail_from = NULL;
	raw_mail.data = NULL;
	raw_mail.rcpt_count = 0;
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

struct mail clean_mail(struct mail mail) {
	free(mail.text);

	for (int i = 0; i < HEADERS_COUNT; i++) {
		struct mail_header current_header = mail.headers[i];
		free(current_header.name);
		free(current_header.value);
		current_header.name = NULL;
		current_header.value = NULL;
	}

	mail.text = NULL;
	mail.headers_count = 0;
}