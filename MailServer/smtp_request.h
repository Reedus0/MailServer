#pragma once

#include "email_address.h"

#define RECIPIENT_COUNT 8
#define SMTP_REQUEST_MAIL_SIZE 8192

struct smtp_request {
	char* domain;
	struct email_address* mail_from;
	struct email_address* rcpt_to_arr[RECIPIENT_COUNT];
	char* data;
	int rcpt_count;
};

struct smtp_request* init_smtp_request();
int smtp_request_add_recipient(struct smtp_request* smtp_request, struct email_address* recipient);
int smtp_request_set_mail_from(struct smtp_request* smtp_request, struct email_address* mail_from);
int smtp_request_set_domain(struct smtp_request* smtp_request, char* domain);
int smtp_request_set_data(struct smtp_request* smtp_request, char* data);
int clean_smtp_request(struct smtp_request* smtp_request);