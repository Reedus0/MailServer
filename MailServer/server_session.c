#include <string.h>
#include <malloc.h>
#include "server_session.h"
#include "header.h"

struct server_session* init_server_session() {
	struct server_session* new_server_session = calloc(1, sizeof(struct server_session));

	new_server_session->hostname = NULL;
	new_server_session->state = DEFAULT;
}

enum STATUS server_session_set_hostname(struct server_session* server_session, char* hostname) {
	server_session->hostname = hostname;
	return STATUS_OK;
}

enum STATUS server_session_set_state(struct server_session* server_session, enum server_states state) {
	server_session->state = state;
	return STATUS_OK;
}

enum STATUS server_session_validate_state(struct server_session* server_session, enum server_states state) {
	if (server_session->state < state) {
		return STATUS_NOT_OK;
	}
	return STATUS_OK;
}

enum STATUS clean_server_session(struct server_session* server_session) {
	free(server_session->hostname);
	
	free(server_session);
}