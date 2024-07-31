#pragma once

#include "list.h"

#define MAX_CONFIG_SIZE 8192
#define CONFIG_MAX_LINE_SIZE 255

struct user {
	char* username;
	struct list list;
};

struct config {
	char* domain;
	char* mail_path;
	char* listen_port;
	struct user* users_list;
	char* hostname;
} config;

void config_parse_buffer(char* buffer);
void config_parse_file(char* filename);
char* config_get_domain();
char* config_get_mail_path();
int config_get_listen_port();
struct user* config_get_users();
char* config_get_hostname();