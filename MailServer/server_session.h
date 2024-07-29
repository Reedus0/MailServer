#pragma once

enum server_states {
	DEFAULT = 0,
	INITIALIZED = 1,
	HAS_MAIL_FROM = 2,
	HAS_RCPT_TO = 3,
};

struct server_session {
	enum server_states state;
	char* hostname;
};

struct server_session* init_server_session();
enum STATUS server_session_set_hostname(struct server_session* server_session, char* hostname);
enum STATUS server_session_set_state(struct server_session* server_session, enum server_states state);
enum STATUS server_session_validate_state(struct server_session* server_session, enum server_states state);
enum STATUS clean_server_session(struct server_session* server_session);