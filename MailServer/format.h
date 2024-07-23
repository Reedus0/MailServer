#pragma once

void parse_headers(struct mail mail, char* mail_text);
void add_header(struct mail mail, char* name, char* value);
void get_mail_text(struct mail mail, char* mail_text);