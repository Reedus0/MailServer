#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "smtp_request.h"
#include "email_address.h"
#include "mail_format.h"
#include "buffer.h"
#include "config.h"
#include "status.h"
#include "server_session.h"

static char* get_full_mail_path(char* recipient) {
	char* mail_path = config_get_mail_path();

	int mail_path_length = strlen(mail_path);
	int recipient_length = strlen(recipient);
	char* full_path = calloc(mail_path_length + recipient_length + 2, sizeof(char));
	flush_to_buffer(full_path, 2, "%s/%s", mail_path, recipient);

	return full_path;
}

static enum STATUS write_mail_to_file(char* recipient, char* buffer) {

	char* full_path = get_full_mail_path(recipient);

	FILE* file_ptr = fopen(full_path, "ab");
	if (file_ptr == NULL) {
		return STATUS_ERROR;
	}

	fprintf(file_ptr, "%s", buffer);
	fclose(file_ptr);
	free(full_path);

	return STATUS_OK;
}

static enum STATUS deliver_mail(char* mail, struct smtp_request_recipient* smtp_request_recipient) {
	write_mail_to_file(smtp_request_recipient->email_address->user, mail);

	return STATUS_OK;
}

void process_smtp_request(struct smtp_request* smtp_request, struct server_session* server_session) {
	struct mail* mail = init_mail();

	mail_parse_headers(mail, smtp_request->data);
	mail_add_timestamp(mail, smtp_request->mail_from);
	mail_add_server_headers(mail, smtp_request);
	mail_add_session_headers(mail, server_session);
	
	struct smtp_request_recipient* last_recipient = smtp_request->rcpt_to_list;
	char* final_text = build_mail(mail);
	while (1) {
		deliver_mail(final_text, last_recipient);
		if (last_recipient->list.prev == NULL) {
			break;
		}
		last_recipient = list_parent(last_recipient->list.prev, struct smtp_request_recipient, list);
	}
	free(final_text);

	clean_mail(mail);
}