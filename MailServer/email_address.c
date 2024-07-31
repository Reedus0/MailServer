#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "buffer.h"
#include "email_address.h"
#include "status.h"

struct email_address* init_email_address() {
	struct email_address* new_email_address = calloc(1, sizeof(struct email_address));
	new_email_address->user = NULL;
	new_email_address->domain = NULL;

	return new_email_address;
}

enum STATUS email_address_set_user(struct email_address* email_address, char* user) {
	email_address->user = user;
	return STATUS_OK;
}

enum STATUS email_address_set_domain(struct email_address* email_address, char* domain) {
	email_address->domain = domain;
	return STATUS_OK;
}

enum STATUS clean_email_address(struct email_address* email_address) {
	free(email_address->user);
	free(email_address->domain);

	free(email_address);
	return STATUS_OK;
}

enum STATUS validate_email_string(char* string) {
	int has_username = strchr(string, '@') != string;
	int has_domain = strchr(string, '@') - string != strlen(string) - 1;
	int has_email_symbol = strchr(string, '@') != NULL;
	int one_email_symbol = strchr(string, '@') == strrchr(string, '@');

	if (has_username && has_domain && has_email_symbol && one_email_symbol) {
		return STATUS_OK;
	}
	return STATUS_NOT_OK;
}

static char* remove_email_arrows(char* string) {
	char first_symbol = *string;
	char last_symbol = *(string + strlen(string) - 1);
	if (first_symbol == '<' && last_symbol == '>') {
		*(string + strlen(string) - 1) = 0;
		return string += 1;
	}
	return string;
}

struct email_address* string_to_email_address(char* string) {
	struct email_address* email_address = calloc(1, sizeof(struct email_address));
	char* separator = strchr(string, '@');

	string = remove_email_arrows(string);

	int user_length = separator - string;
	char* user = calloc(user_length + 1, sizeof(char));
	memcpy(user, string, user_length);
	email_address->user = user;

	int domain_length = strlen(string) - (separator - string);
	char* domain = calloc(domain_length + 1, sizeof(char));
	memcpy(domain, separator + 1, domain_length);
	email_address->domain = domain;

	return email_address;
}

char* email_address_to_string(struct email_address* email_address) {
	int user_length = strlen(email_address->user);
	int domain_length = strlen(email_address->domain);

	char* result = calloc(user_length + domain_length + 2, sizeof(char));

	flush_to_buffer(result, 2, "%s@%s", email_address->user, email_address->domain);

	return result;
}