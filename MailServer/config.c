#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "config.h"

struct config config_parse_file(char* filename) {
	struct config new_config;
	new_config.domain = "domain.local";
	new_config.hostname = "smtp";
	new_config.mail_path = ".";
	new_config.listen_port = 1025;
	new_config.users = calloc(MAX_USERS, sizeof(char*));

	new_config.users[0] = "john";
	new_config.users[1] = "carl";

	return new_config;
}

char* config_get_domain(struct config* config) {
	return config->domain;
}

char* config_get_hostname(struct config* config) {
	return config->hostname;
}

char* config_get_mail_path(struct config* config) {
	return config->mail_path;
}

char** config_get_users(struct config* config) {
	return config->users;
}

char* config_get_listen_port(struct config* config) {
	return config->listen_port;
}