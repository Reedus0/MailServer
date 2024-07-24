#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mail.h"
#include "message.h"
#include "format.h"

static void write_mail_to_file(char* filename, char* buffer) {
	FILE* file_ptr = fopen(filename, "a");

	fprintf(file_ptr, "%s", buffer);

	fclose(file_ptr);
}

void deliver_mail(struct raw_mail mail) {
	struct mail result_mail = init_mail();

	result_mail = parse_headers(result_mail, mail.data);
	result_mail = get_mail_text(result_mail, mail.data);
	
	for (int i = 0; i < mail.rcpt_count; i++) {
		char* recipient = get_email_user(mail.rcpt_to_arr[i]);
		if (recipient != NULL) {
			char* final_text = build_mail(result_mail);
			write_mail_to_file(recipient, final_text);
			free(final_text);
		}
	}

	clean_mail(result_mail);
}
