#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <WinSock2.h>
#include "config.h"
#include "buffer.h"
#include "status.h"

static struct user* init_user() {
	struct user* new_user = calloc(1, sizeof(struct user));

	new_user->username = NULL;
	new_user->list = init_list();

	return new_user;
}

static enum STATUS clean_user(struct user* user) {
	free(user->username);

	free(user);

	return STATUS_OK;
}

static void throw_config_error(char* error) {
	printf("Error parsing config: %s", error);
	exit(1);
}

static char* read_config_file(char* filename) {
	char* buffer = calloc(MAX_CONFIG_SIZE, sizeof(char));

	FILE* file_ptr = fopen(filename, "r");
	if (file_ptr == NULL) {
		throw_config_error("No config file");
	}

	while (fgets(buffer + strlen(buffer), CONFIG_MAX_LINE_SIZE, file_ptr));

	char* last_symbol = (buffer + strlen(buffer) - 1);
	while (*last_symbol == '\n') {
		*last_symbol = '\0';
		last_symbol -= 1;
	}

	fclose(file_ptr);

	return buffer;
}

static char* get_config_param(char* config, char* param, char* default_param) {
	char* current_line;
	char* iterator = config;

	while (iterator != NULL) {
		current_line = buffer_get_next(&iterator, "\n");
		char* line_param = trim_string(get_field_from_buffer(current_line, "="));

		if (compare_strings(line_param, param)) {
			char* result = trim_string(get_value_from_buffer(current_line, "="));
			free(current_line);
			free(line_param);
			return result;
		}

		free(current_line);
		free(line_param);
	}

	return default_param;
}

static struct user* get_config_users(char* config, char* default_param) {
	char* config_users = get_config_param(config, "users", default_param);

	char* iterator = config_users;

	struct user* last_user = init_user();
	last_user->username = buffer_get_next(&iterator, ",");

	while (iterator != NULL) {
		char* current_user = buffer_get_next(&iterator, ",");

		struct user* new_user = init_user();
		new_user->username = current_user;

		list_insert(&last_user->list, &new_user->list);
		last_user = new_user;
	}

	return last_user;
}

static char* get_config_hostname() {
	char* hostname = calloc(256, sizeof(char)); // Max hostname length is 255
	int status = gethostname(hostname, 256);
	if (status == -1) {
		throw_config_error("Couldn't get hostname");
	}
	lower_buffer(hostname);
	return hostname;
}

void config_parse_buffer(char* buffer) {
	config.domain = get_config_param(buffer, "domain", "domain.local");
	config.mail_path = get_config_param(buffer, "mail_path", "./");
	config.listen_port = get_config_param(buffer, "listen_port", "25");
	config.users_list = get_config_users(buffer, "");
	config.hostname = get_config_hostname();
}

void config_parse_file(char* filename) {
	char* buffer = read_config_file(filename);

	config_parse_buffer(buffer);

	free(buffer);
}

char* config_get_domain() {
	return config.domain;
}

char* config_get_mail_path() {
	return config.mail_path;
}

int config_get_listen_port() {
	return atoi(config.listen_port);
}

struct user* config_get_users() {
	return config.users_list;
}

char* config_get_hostname() {
	return config.hostname;
}