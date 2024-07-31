#pragma once

struct server_session {
	char* hostname;
};

struct server_session* init_server_session();
enum STATUS server_session_set_hostname(struct server_session* server_session, char* hostname);
enum STATUS clean_server_session(struct server_session* server_session);