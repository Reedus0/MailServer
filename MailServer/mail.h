#pragma once

#include "list.h"

#define MAIL_SIZE 8192
#define TIMESTAMP_SIZE 128

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
int mail_add_header(struct mail* mail, char* name, char* value);
int mail_set_text(struct mail* mail, char* text);
int mail_set_timestamp(struct mail* mail, char* timestamp);
char* build_mail(struct mail* mail);
int clean_mail(struct mail* mail);