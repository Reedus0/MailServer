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
struct raw_mail clean_raw_mail(struct raw_mail raw_mail);

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
struct mail clean_mail(struct mail mail);