#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "config.h"
#include "buffer.h"

void config_parse_file(char* filename) {
	config.domain = "domain.local";
	config.mail_path = ".";
	config.listen_port = 1025;
	config.users = "john";
}

char* config_get_domain() {
	return config.domain;
}

char* config_get_mail_path() {
	return config.mail_path;
}

char* config_get_users() {
	return config.users;
}

char* config_get_listen_port() {
	return config.listen_port;
}