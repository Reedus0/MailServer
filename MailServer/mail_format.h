#pragma once

#define DEFAULT_HEADER_SIZE 1024
#define TIMESTAMP_SIZE 128

enum STATUS mail_parse_headers(struct mail* mail, char* mail_text);
enum STATUS mail_add_timestamp(struct mail* mail, struct email_address* mail_from);
enum STATUS mail_add_server_headers(struct mail* mail, struct smtp_request* smtp_request);
enum STATUS mail_add_session_headers(struct mail* mail, struct server_session* server_session);