#pragma once

#include "email_address.h"
#include "list.h"

struct smtp_request_recipient {
	struct email_address* email_address;
	struct list list;
};

struct smtp_request {
	struct email_address* mail_from;
	struct smtp_request_recipient* rcpt_to_list;
	char* data;
};

struct smtp_request* init_smtp_request();
enum STATUS smtp_request_add_recipient(struct smtp_request* smtp_request, struct email_address* recipient);
enum STATUS smtp_request_set_mail_from(struct smtp_request* smtp_request, struct email_address* mail_from);
enum STATUS smtp_request_set_data(struct smtp_request* smtp_request, char* data);
enum STATUS clean_smtp_request(struct smtp_request* smtp_request);