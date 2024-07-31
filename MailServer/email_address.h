#pragma once

struct email_address {
	char* user;
	char* domain;
};

struct email_address* init_email_address();
enum STATUS validate_email_string(char* string);
struct email_address* string_to_email_address(char* string);
char* email_address_to_string(struct email_address* email_address);
enum STATUS email_address_set_user(struct email_address* email_address, char* user);
enum STATUS email_address_set_domain(struct email_address* email_address, char* domain);
enum STATUS clean_email_address(struct email_address* email_address);