#pragma once

#define HEADERS_COUNT 16
#define MAIL_SIZE 8192
#define TIMESTAMP_SIZE 128

struct mail_header {
	char* name;
	char* value;
};

struct mail {
	struct mail_header headers[HEADERS_COUNT];
	int headers_count;
	char* timestamp;
	char* text;
};

struct mail init_mail();
int mail_add_header(struct mail* mail, char* name, char* value);
int mail_set_text(struct mail* mail, char* text);
int mail_set_timestamp(struct mail* mail, char* timestamp);
char* build_mail(struct mail* mail);
int clean_mail(struct mail* mail);