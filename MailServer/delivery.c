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

	parse_headers(result_mail, mail.data);
	get_mail_text(result_mail, mail.data);
	
	for (int i = 0; i < mail.rcpt_count; i++) {
		char* recipient = mail.rcpt_to_arr[i];
		if (recipient != NULL) {
			add_header(result_mail, "Dilivered to", recipient);
			write_mail_to_file(recipient, build_mail(result_mail));
		}
	}
}
