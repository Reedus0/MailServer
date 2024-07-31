#include <string.h>
#include <malloc.h>
#include "server_session.h"
#include "status.h"

struct server_session* init_server_session() {
	struct server_session* new_server_session = calloc(1, sizeof(struct server_session));

	new_server_session->hostname = NULL;
	return new_server_session;
}

enum STATUS server_session_set_hostname(struct server_session* server_session, char* hostname) {
	server_session->hostname = hostname;
	return STATUS_OK;
}

enum STATUS clean_server_session(struct server_session* server_session) {
	free(server_session->hostname);
	
	free(server_session);

	return STATUS_OK;
}