#include <string.h>
#include <malloc.h>
#include "smtp_request.h"
#include "email_address.h"
#include "server_session.h"

struct smtp_request* make_smtp_request(char* mail_from, char* rcpt_to, char* data) {
    struct smtp_request* smtp_request = init_smtp_request();

    char* smtp_request_data = calloc(strlen(data) + 1, sizeof(char));
    memcpy(smtp_request_data, data, strlen(data));
    smtp_request_set_data(smtp_request, smtp_request_data);

    smtp_request_set_mail_from(smtp_request, string_to_email_address(mail_from));
    smtp_request_add_recipient(smtp_request, string_to_email_address(rcpt_to));
    return smtp_request;
}

struct email_address* make_email_address(char* address) {
    char* email_address_string = calloc(strlen(address) + 1, sizeof(char));
    memcpy(email_address_string, address, strlen(address));
    return string_to_email_address(email_address_string);
}

struct server_session* make_server_session(char* hostname) {
    struct server_session* server_session = init_server_session();

    char* server_session_hostname = calloc(strlen(hostname) + 1, sizeof(char));
    memcpy(server_session_hostname, hostname, strlen(hostname));
    server_session_set_hostname(server_session, server_session_hostname);

    return server_session;
}