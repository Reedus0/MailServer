#pragma once

#define MAX_USERS 16

struct config {
	char* domain;
	char* mail_path;
	char* users;
	short listen_port;
};

struct config config_parse_file(char* filename);
char* config_get_domain(struct config* config);
char* config_get_mail_path(struct config* config);
char* config_get_users(struct config* config);
char* config_get_listen_port(struct config* config);