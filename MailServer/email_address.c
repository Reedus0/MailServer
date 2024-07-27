#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "buffer.h"
#include "email_address.h"

struct email_address* init_email_address() {
	struct email_address* new_email_address = calloc(1, sizeof(struct email_address));
	new_email_address->user = NULL;
	new_email_address->domain = NULL;

	return new_email_address;
}

int validate_email_string(char* string) {
	int has_username = strchr(string, '@') != string;
	int has_domain = strchr(string, '@') - string != strlen(string) - 1;
	int has_email_symbol = strchr(string, '@') != NULL;
	int one_email_symbol = strchr(string, '@') == strrchr(string, '@');

	return has_username && has_domain && has_email_symbol && one_email_symbol;
}

struct email_address* string_to_email_address(char* string) {
	struct email_address* email_address = calloc(1, sizeof(struct email_address));
	char* separator = strchr(string, '@');

	if (*string == '<') {
		*(string + strlen(string) - 1) = 0;
		string += 1;
	}

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
	int domain_length = strlen(email_address->domain) + 1;

	char* result = calloc(user_length + domain_length + 2, sizeof(char));

	flush_to_buffer(result, user_length + domain_length + 1, "%s@%s", email_address->user, email_address->domain);

	return result;
}

int clean_email_address(struct email_address* email_address) {
	free(email_address->user);
	free(email_address->domain);

	free(email_address);
	return 1;
}