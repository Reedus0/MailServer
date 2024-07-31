#pragma once

#include "list.h"

#define MAIL_SIZE 8192

struct mail_header {
	char* name;
	char* value;
	struct list list;
};

struct mail {
	char* text;
	char* timestamp;
	struct mail_header* headers_list;
};

struct mail* init_mail();
enum STATUS mail_add_header(struct mail* mail, char* name, char* value);
char* mail_get_header_value(struct mail* mail, char* name);
enum STATUS mail_has_header(struct mail* mail, char* name);
enum STATUS mail_add_header_if_not_exists(struct mail* mail, char* name, char* value);
enum STATUS mail_remove_header(struct mail* mail, char* name);
enum STATUS mail_replace_header(struct mail* mail, char* name, char* value);
enum STATUS mail_set_text(struct mail* mail, char* text);
enum STATUS mail_set_timestamp(struct mail* mail, char* timestamp);
char* build_mail(struct mail* mail);
enum STATUS clean_mail(struct mail* mail);