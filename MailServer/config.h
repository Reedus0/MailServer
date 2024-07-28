#pragma once

#define MAX_USERS 16
#define MAX_CONFIG_SIZE 8192
#define CONFIG_MAX_LINE_SIZE 255

struct config {
	char* domain;
	char* mail_path;
	char* users;
	short listen_port;
} config;

void config_parse_file(char* filename);
char* config_get_domain();
char* config_get_mail_path();
char* config_get_users();
char* config_get_listen_port();