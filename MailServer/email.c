#include <string.h>
#include <stdarg.h>
#include <malloc.h>

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
	return has_username && has_domain && has_email_symbol && one_email_symbol;
}