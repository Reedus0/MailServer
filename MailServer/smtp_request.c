#include <stdio.h>
#include <malloc.h>
#include "smtp_request.h"
#include "email_address.h"

struct smtp_request* init_smtp_request() {
	struct smtp_request* new_smtp_request = calloc(1, sizeof(struct smtp_request));
	new_smtp_request->mail_from = init_email_address();
	new_smtp_request->domain = NULL;
	new_smtp_request->data = NULL;
	new_smtp_request->rcpt_count = 0;

	for (int i = 0; i < RECIPIENT_COUNT; i++) {
		struct email_address* current_recipient = new_smtp_request->rcpt_to_arr[i];
		init_email_address(current_recipient);
	}

	return new_smtp_request;
}

int smtp_request_add_recipient(struct smtp_request* smtp_request, struct email_address* recipient) {
	if (smtp_request->rcpt_count >= RECIPIENT_COUNT) return 0;

	smtp_request->rcpt_to_arr[smtp_request->rcpt_count] = recipient;
	smtp_request->rcpt_count += 1;
	return 1;
}

int smtp_request_set_mail_from(struct smtp_request* smtp_request, struct email_address* mail_from) {
	smtp_request->mail_from = mail_from;
	return 1;
}
int smtp_request_set_domain(struct smtp_request* smtp_request, char* domain) {
	smtp_request->domain = domain;
	return 1;
}
int smtp_request_set_data(struct smtp_request* smtp_request, char* data) {
	smtp_request->data = data;
	return 1;
}

int clean_smtp_request(struct smtp_request* smtp_request) {
	clean_email_address(smtp_request->mail_from);
	free(smtp_request->domain);
	free(smtp_request->data);

	for (int i = 0; i < smtp_request->rcpt_count; i++) {
		struct email_address* current_recipient = smtp_request->rcpt_to_arr[i];
		clean_email_address(current_recipient);
		smtp_request->rcpt_to_arr[i] = NULL;
	}

	smtp_request->mail_from = NULL;
	smtp_request->domain = NULL;
	smtp_request->data = NULL;
	smtp_request->rcpt_count = 0;

	free(smtp_request);
	return 1;
}