#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include "header.h"
#include "server.h"
#include "config.h"
#pragma comment(lib, "ws2_32.lib")

#if DEBUG_DELIVERY == 1
#include "smtp_request.h"
#include "delivery.h"
#include "email_address.h"
#endif

int main(int argv, char* argc[]) {

#if DEBUG_DELIVERY == 1
    struct smtp_request* smtp_request = init_smtp_request();

    char* _data = "Subject: Mail subject\nDate: now\n\nMail text\n";
    char* data = calloc(strlen(_data) + 1, sizeof(char));
    memcpy(data, _data, strlen(_data));
    smtp_request->data = data;

    char* _mail_from = "john@domain.local";
    char* mail_from = calloc(strlen(_mail_from) + 1, sizeof(char));
    memcpy(mail_from, _mail_from, strlen(_mail_from));
    smtp_request->mail_from = string_to_email_address(_mail_from);

    char* _rcpt_to = "john@domain.local";
    char* rcpt_to = calloc(strlen(_rcpt_to) + 1, sizeof(char));
    memcpy(rcpt_to, _rcpt_to, strlen(_rcpt_to));
    smtp_request->rcpt_to_arr[0] = string_to_email_address(rcpt_to);
    smtp_request->rcpt_count = 1;

    deliver_mail(smtp_request);
    clean_smtp_request(smtp_request);

    return 0;
#endif

    thrd_t new_thread;
    WSADATA wsa_data;

    struct config config = config_parse_file("config.txt");

    int status = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (status != 0) {
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(config_get_listen_port(&config));

    bind(sock, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(sock, MAX_PENDING_CONNECTIONS);

    while (1) {
        SOCKET new_sock = accept(sock, NULL, NULL);
        if (new_sock == -1) {
            continue;
        }
        thrd_create(&new_thread, serve_connection, new_sock);
    }

	return 0;
}