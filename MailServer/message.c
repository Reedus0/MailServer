#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include "codes.h"

void add_to_message(char* buffer, char* message) {
	memcpy(buffer + strlen(buffer), message, strlen(message) + 1);
}

char* get_field_from_message(char* message, int offset) {
	char* result = calloc(offset + 1, sizeof(char));
	memcpy(result, message, offset);
	return result;
}

char* get_value_from_message(char* message, int offset) {
	int new_length = strlen(message) - offset - 1;
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, message + offset + 1, new_length - 1);
	return result;
}

int check_message_command(char* command, char* message) {
	return !memcmp(message, command, strlen(command));
}

char* get_email_user(char* email) {
	char* separator = strchr(email, '@');
	char* result = calloc(separator - email + 1, sizeof(char));
	memcpy(result, email, separator - email);
	return result;
}

char* get_email_domain(char* email) {
	char* separator = strchr(email, '@');
	char* result = calloc(strlen(email) - (separator - email) + 1, sizeof(char));
	memcpy(result, separator + 1, strlen(email) - (separator - email));
	return result;
}

char* validate_email(char* email) {
	int has_username = strchr(email, '@') != email;
	int has_domain = strchr(email, '@') - email != strlen(email) - 1;
	int has_email_symbol = strchr(email, '@') != NULL;
	int one_email_symbol = strchr(email, '@') == strrchr(email, '@');
	return has_username && has_domain  && has_email_symbol && one_email_symbol;
}