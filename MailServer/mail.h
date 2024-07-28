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
enum STATUS mail_add_header(struct mail* mail, char* name, char* value);
enum STATUS mail_set_text(struct mail* mail, char* text);
enum STATUS mail_set_timestamp(struct mail* mail, char* timestamp);
char* build_mail(struct mail* mail);
enum STATUS clean_mail(struct mail* mail);