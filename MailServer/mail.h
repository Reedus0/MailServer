#pragma once

#define RECIPIENT_COUNT 8
#define HEADERS_COUNT 16
#define MAIL_SIZE 8192

struct raw_mail {
	char* domain;
	char* mail_from;
	char* rcpt_to_arr[RECIPIENT_COUNT];
	char* data;
	int rcpt_count;
};

struct raw_mail init_raw_mail();
int clean_raw_mail(struct raw_mail* raw_mail);
int raw_mail_add_recipient(struct raw_mail* raw_mail, char* recipient);

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