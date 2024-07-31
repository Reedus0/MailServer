#include <threads.h>
#include <Winsock2.h>
#include "header.h"
#include "server.h"
#include "config.h"
#pragma comment(lib, "ws2_32.lib")

int main(int argv, char* argc[]) {

    thrd_t new_thread;
    WSADATA wsa_data;

    int status = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (status != 0) {
        return 1;
    }

    if (argv < 2) {
        printf("Usage: %s config_file_path", argc[0]);
        return 1;
    }

    config_parse_file(argc[1]);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(config_get_listen_port());

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