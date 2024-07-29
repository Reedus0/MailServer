#include <stdio.h>
#include <malloc.h>
#include "mail.h"
#include "buffer.h"
#include "header.h"

static struct mail_header* init_mail_header() {
	struct mail_header* new_mail_header = calloc(1, sizeof(struct mail_header));
	new_mail_header->name = NULL;
	new_mail_header->value = NULL;

	return new_mail_header;
}

struct mail* init_mail() {
	struct mail* new_mail = calloc(1, sizeof(struct mail));
	new_mail->text = NULL;
	new_mail->timestamp = NULL;
	new_mail->headers_list = NULL;

	return new_mail;
}

enum STATUS mail_add_header(struct mail* mail, char* name, char* value) {
	struct mail_header* new_mail_header = init_mail_header();

	new_mail_header->name = copy_buffer(name);
	new_mail_header->value = copy_buffer(value);

	if (mail->headers_list == NULL) {
		mail->headers_list = new_mail_header;
		return STATUS_OK;
	}

	struct mail_header* last_header = mail->headers_list;
	list_insert(&last_header->list, &new_mail_header->list);

	mail->headers_list = new_mail_header;

	return STATUS_OK;
}

char* mail_get_header_value(struct mail* mail, char* name) {
	struct mail_header* current_header = mail->headers_list;
	if (current_header == NULL) {
		return STATUS_NOT_OK;
	}
	while (1) {
		if (compare_strings(current_header->name, name)) {
			return current_header->value;
		}
		if (current_header->list.prev == NULL) {
			break;
		}
		current_header = list_parent(current_header->list.prev, struct mail_header, list);
	}
	return NULL;
}

enum STATUS mail_has_header(struct mail* mail, char* name) {
	if (mail_get_header_value(mail, name) == NULL) {
		return STATUS_NOT_OK;
	}
	return STATUS_OK;
}

enum STATUS mail_add_header_if_not_exists(struct mail* mail, char* name, char* value) {
	if (!mail_has_header(mail, name)) {
		mail_add_header(mail, name, value);
	}
	return STATUS_OK;
}

enum STATUS mail_set_text(struct mail* mail, char* text) {
	mail->text = copy_buffer(text);
	return STATUS_OK;
}

enum STATUS mail_set_timestamp(struct mail* mail, char* timestamp) {
	mail->timestamp = copy_buffer(timestamp);
	return STATUS_OK;
}

char* build_mail(struct mail* mail) {
	char* result = calloc(MAIL_SIZE, sizeof(char));
	add_to_buffer(result, mail->timestamp);

	struct mail_header* current_header = mail->headers_list;

	while (1) {
		int name_length = strlen(current_header->name);
		int value_length = strlen(current_header->value);
		flush_to_buffer(result, name_length + value_length + 5, "%s: %s\r\n", current_header->name, current_header->value);
		if (current_header->list.prev == NULL) {
			break;
		}
		current_header = list_parent(current_header->list.prev, struct mail_header, list);
	}

	int text_length = strlen(mail->text);
	flush_to_buffer(result, text_length + 5, "\r\n%s\r\n", mail->text);

	return result;
}

static enum STATUS clean_mail_header(struct mail_header* mail_header) {
	free(mail_header->name);
	free(mail_header->value);

	free(mail_header);
	return STATUS_OK;
}

enum STATUS clean_mail(struct mail* mail) {
	if (mail->headers_list != NULL) {
		struct mail_header* current_header = mail->headers_list;
		struct mail_header* prev_header = list_parent(current_header->list.prev, struct mail_header, list);

		while (1) {
			if (current_header->list.prev == NULL) {
				clean_mail_header(current_header);
				break;
			}
			clean_mail_header(current_header);
			current_header = prev_header;
			prev_header = list_parent(prev_header->list.prev, struct mail_header, list);
		}
	}

	free(mail->text);
	free(mail->timestamp);

	free(mail);
	return STATUS_OK;
}