#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "smtp_request.h"
#include "email_address.h"
#include "mail_format.h"
#include "buffer.h"
#include "config.h"
#include "header.h"

static enum STATUS write_mail_to_file(char* recipient, char* buffer) {
	char* mail_path = config_get_mail_path();

	int mail_path_length = strlen(mail_path);
	int recipient_length = strlen(recipient);
	char* full_path = calloc(mail_path_length + recipient_length + 3, sizeof(char));
	flush_to_buffer(full_path, mail_path_length + recipient_length + 2, "%s/%s", mail_path, recipient);

	FILE* file_ptr = fopen(full_path, "ab");
	if (file_ptr == NULL) {
		return STATUS_ERROR;
	}

	fprintf(file_ptr, "%s", buffer);
	fclose(file_ptr);
	free(full_path);

	return STATUS_OK;
}

void deliver_mail(struct smtp_request* smtp_request) {
	struct mail* mail = init_mail();

	format_mail(mail, smtp_request);
	
	struct smtp_request_recipient* last_recipient = smtp_request->rcpt_to_list;
	while (1) {
		if (last_recipient->email_address->user != NULL && last_recipient->email_address->domain != NULL) {
			char* final_text = build_mail(mail);
			write_mail_to_file(last_recipient->email_address->user, final_text);
			free(final_text);
		}
		if (last_recipient->list.next == NULL) {
			break;
		}
		last_recipient = list_parent(last_recipient->list.next, struct smtp_request_recipient, list);
	}

	clean_mail(mail);
}
