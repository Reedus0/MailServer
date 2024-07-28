#include <stdio.h>
#include <malloc.h>
#include "smtp_request.h"
#include "email_address.h"
#include "list.h"

struct smtp_request* init_smtp_request() {
	struct smtp_request* new_smtp_request = calloc(1, sizeof(struct smtp_request));
	new_smtp_request->domain = NULL;
	new_smtp_request->data = NULL;
	new_smtp_request->mail_from = NULL;
	new_smtp_request->rcpt_to_list = NULL;

	return new_smtp_request;
}

static struct smtp_request_recipient* init_smtp_request_recipient() {
	struct smtp_request_recipient* new_smtp_request_recipient = calloc(1, sizeof(struct smtp_request_recipient));

	new_smtp_request_recipient->list = init_list();
	new_smtp_request_recipient->email_address = NULL;

	return new_smtp_request_recipient;
}

int smtp_request_add_recipient(struct smtp_request* smtp_request, struct email_address* recipient) {
	struct smtp_request_recipient* new_smtp_request_recipient = init_smtp_request_recipient();

	new_smtp_request_recipient->email_address = recipient;

	if (smtp_request->rcpt_to_list == NULL) {
		smtp_request->rcpt_to_list = new_smtp_request_recipient;
		return 1;
	}

	struct smtp_request_recipient* last_recipient = smtp_request->rcpt_to_list;
	list_insert(&last_recipient->list, &new_smtp_request_recipient->list);

	smtp_request->rcpt_to_list = new_smtp_request_recipient;
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

static int clean_smtp_request_recipient(struct smtp_request_recipient* smtp_request_recipient) {
	if (smtp_request_recipient->email_address != NULL) clean_email_address(smtp_request_recipient->email_address);

	free(smtp_request_recipient);
	return 1;
}

int clean_smtp_request(struct smtp_request* smtp_request) {
	if (smtp_request->mail_from != NULL) {
		clean_email_address(smtp_request->mail_from);
	}

	if (smtp_request->rcpt_to_list != NULL) {
		struct smtp_request_recipient* current_recipient = smtp_request->rcpt_to_list;
		struct smtp_request_recipient* prev_recipient = list_parent(current_recipient->list.prev, struct smtp_request_recipient, list);

		while (1) {
			if (current_recipient->list.prev == NULL) {
				clean_smtp_request_recipient(current_recipient);
				break;
			}
			clean_smtp_request_recipient(current_recipient);
			current_recipient = prev_recipient;
			prev_recipient = list_parent(prev_recipient->list.prev, struct smtp_request_recipient, list);
		}
	}

	free(smtp_request->domain);
	free(smtp_request->data);

	free(smtp_request);
	return 1;
}