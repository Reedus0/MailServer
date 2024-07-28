#include <stdio.h>
#include "smtp_request.h"
#include "email_address.h"
#include "func.h"
#include "make.h"
#include "delivery.h"

int test_add_multiple_recipients() {
	struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "Mail text\r\n");

	smtp_request_add_recipient(smtp_request, string_to_email_address("carl@domain.local"));
	smtp_request_add_recipient(smtp_request, string_to_email_address("ivan@domain.local"));
	smtp_request_add_recipient(smtp_request, string_to_email_address("vlad@domain.local"));

	struct smtp_request_recipient* current_recipient = smtp_request->rcpt_to_list;
	int i = 0;

	while (1) {
		i += 1;
		if (current_recipient->list.prev == NULL) {
			break;
		}
		current_recipient = list_parent(current_recipient->list.prev, struct smtp_request_recipient, list);
	}

	if (int_except_eq(i, 4)) {
		printf("test_add_multiple_recipients OK\n");
		return 1;
	}
    return 0;
}