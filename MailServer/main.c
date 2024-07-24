#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include "header.h"
#include "server.h"
#include "mail.h"
#include "delivery.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argv, char* argc[]) {

    thrd_t new_thread;
    WSADATA wsa_data;

    //struct raw_mail mail = init_raw_mail();
    //mail.data = "Subject: Mail subject\nDate: now\n\nMail text\n";
    //mail.data = "Mail text\n";
    //mail.mail_from = "john@domain";
    //mail.rcpt_to_arr[0] = "john@domain";
    //mail.rcpt_count = 1;
    //deliver_mail(mail);
    //return 0;

    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(LISTEN_PORT);

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