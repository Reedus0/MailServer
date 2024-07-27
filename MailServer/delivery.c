#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "smtp_request.h"
#include "email_address.h"
#include "format.h"
#include "buffer.h"
#include "config.h"

static int write_mail_to_file(char* recipient, char* buffer) {
	struct config config = config_parse_file("config.txt");
	char* mail_path = config_get_mail_path(&config);

	int mail_path_length = strlen(mail_path);
	int recipient_length = strlen(recipient) + 1;
	char* full_path = calloc(mail_path_length + recipient_length + 2, sizeof(char));
	flush_to_buffer(full_path, mail_path_length + recipient_length + 1, "%s/%s", mail_path, recipient);

	FILE* file_ptr = fopen(full_path, "a");

	if (file_ptr == NULL) {
		return -1;
	}
	fprintf(file_ptr, "%s", buffer);
	fclose(file_ptr);
	free(full_path);

	return 1;
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
