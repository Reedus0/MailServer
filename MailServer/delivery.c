#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "smtp_request.h"
#include "email_address.h"
#include "format.h"
#include "message.h"

static void write_mail_to_file(char* filename, char* buffer) {
	FILE* file_ptr = fopen(filename, "a");

	fprintf(file_ptr, "%s", buffer);

	fclose(file_ptr);
}

void deliver_mail(struct smtp_request smtp_request) {
	struct mail mail = init_mail();

	format_mail(&mail, &smtp_request);
	
	for (int i = 0; i < smtp_request.rcpt_count; i++) {
		struct email_address recipient = smtp_request.rcpt_to_arr[i];

		if (recipient.user != NULL && recipient.domain != NULL) {
			char* final_text = build_mail(&mail);
			write_mail_to_file(recipient.user, final_text);
			free(final_text);
		}

	}

	clean_mail(&mail);
}
