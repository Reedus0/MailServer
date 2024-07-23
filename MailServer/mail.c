#include <stdio.h>
#include "mail.h"

struct raw_mail init_raw_mail() {
	struct raw_mail new_raw_mail;
	new_raw_mail.domain = NULL;
	new_raw_mail.mail_from = NULL;
	new_raw_mail.data = NULL;
	new_raw_mail.rcpt_count = 0;

	for (int i = 0; i < RECIPIENT_COUNT; i++) {
		new_raw_mail.rcpt_to_arr[i] = NULL;
	}

	return new_raw_mail;
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

char* build_mail(struct mail mail) {
	return mail.text;
}