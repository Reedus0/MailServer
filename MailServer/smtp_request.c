#include <stdio.h>
#include <malloc.h>
#include "smtp_request.h"
#include "email_address.h"
#include "list.h"

struct smtp_request* init_smtp_request() {
	struct smtp_request* new_smtp_request = calloc(1, sizeof(struct smtp_request));
	new_smtp_request->mail_from = NULL;
	new_smtp_request->domain = NULL;
	new_smtp_request->data = NULL;
	new_smtp_request->rcpt_to_list = NULL;

	return new_smtp_request;
}

struct smtp_request_recipient* init_smtp_requset_recipient() {
	struct smtp_request_recipient* new_smtp_requset_recipient = calloc(1, sizeof(struct smtp_request_recipient));

	new_smtp_requset_recipient->list = init_list();
	new_smtp_requset_recipient->email_address = NULL;

	return new_smtp_requset_recipient;
}

int smtp_request_add_recipient(struct smtp_request* smtp_request, struct email_address* recipient) {
	struct smtp_request_recipient* new_smtp_requset_recipient = init_smtp_requset_recipient();

	new_smtp_requset_recipient->email_address = recipient;

	if (smtp_request->rcpt_to_list == NULL) {
		smtp_request->rcpt_to_list = new_smtp_requset_recipient;
		return 1;
	}

	struct smtp_request_recipient* last_recipient = smtp_request->rcpt_to_list;

	while (last_recipient->list.next != NULL) {
		last_recipient = list_parent(last_recipient->list.next, struct smtp_request_recipient, list);
	}

	list_insert(&last_recipient->list, &new_smtp_requset_recipient->list);
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

	free(smtp_request);
	return 1;
}