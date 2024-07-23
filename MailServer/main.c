#include <stdio.h>
#include <threads.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include "header.h"
#include "server.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argv, char* argc[]) {

    mtx_t mutex;
    thrd_t worker_threads[MAX_CONNECTIONS];
    WSADATA wsa_data;

    mtx_init(&mutex, mtx_plain);

    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(1025);

    bind(sock, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(sock, MAX_CONNECTIONS);

    while (1) {
        SOCKET new_sock = accept(sock, NULL, NULL);
        if (new_sock == -1) {
            continue;
        }
        thrd_create(&worker_threads[0], serve_connection, new_sock);
        thrd_join(worker_threads[0], NULL);
    }

	// thrd_create(&worker_threads[0], hello, NULL);
	// thrd_join(worker_threads[0], NULL);
	return 0;
}