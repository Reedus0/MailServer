#pragma once

struct email_address {
	char* user;
	char* domain;
};

struct email_address init_email_address();
int validate_email_string(char* string);
struct email_address string_to_email_address(char* string);
char* email_address_to_string(struct email_address* email_address);
int clean_email_address(struct email_address* email_address);