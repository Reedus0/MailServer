#pragma once

struct smtp_request* make_smtp_request(char* domain, char* mail_from, char* rcpt_to, char* data);