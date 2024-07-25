#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include "header.h"
#include "server.h"
#include "smtp_request.h"
#include "delivery.h"
#include "config.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argv, char* argc[]) {

    thrd_t new_thread;
    WSADATA wsa_data;

    struct config config = config_parse_file("config.txt");

    //struct smtp_request smtp_request = init_smtp_request();
    //smtp_request.data = "Subject: Mail subject\nDate: now\n\nMail text\n";
    //smtp_request.data = "Mail text\n";
    //smtp_request.mail_from = string_to_email_address("john@domain.local");
    //smtp_request.rcpt_to_arr[0] = string_to_email_address("john@domain.local");
    //smtp_request.rcpt_count = 1;
    //deliver_mail(smtp_request);
    //clean_smtp_request(&smtp_request);
    //return 0;

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