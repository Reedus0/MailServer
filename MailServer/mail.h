#pragma once

#define HEADERS_COUNT 16
#define MAIL_SIZE 8192

struct mail_header {
	char* name;
	char* value;
};

struct mail {
	struct mail_header headers[HEADERS_COUNT];
	char* text;
	int headers_count;
};

struct mail init_mail();
int mail_add_header(struct mail* mail, char* name, char* value);
int mail_set_text(struct mail* mail, char* text);
char* build_mail(struct mail* mail);
int clean_mail(struct mail* mail);