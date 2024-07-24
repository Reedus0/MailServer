#pragma once

struct mail parse_headers(struct mail mail, char* mail_text);
struct mail add_header(struct mail mail, char* name, char* value);
struct mail get_mail_text(struct mail mail, char* mail_text);
char* build_mail(struct mail mail);