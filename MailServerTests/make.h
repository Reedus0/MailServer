#pragma once

struct smtp_request* make_smtp_request(char* hostname, char* mail_from, char* rcpt_to, char* data);
struct email_address* make_email_address(char* address);