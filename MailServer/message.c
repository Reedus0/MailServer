#include <string.h>
#include <stdarg.h>
#include <malloc.h>

void add_to_message(char* buffer, char* message) {
	memcpy(buffer + strlen(buffer), message, strlen(message) + 1);
}

char* get_field_from_message(char* message, int offset) {
	char* result = calloc(offset + 1, sizeof(char));
	memcpy(result, message, offset);
	return result;
}

char* get_value_from_message(char* message, int offset) {
	int new_length = strlen(message) - offset - 1;
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, message + offset + 1, new_length - 1);
	return result;
}

int check_message_command(char* command, char* message) {
	return !memcmp(message, command, strlen(command));
}

char* trim_string(char* string) {
	while (isspace(*string)) string++;
	return string;
}

int is_empty_string(char* string) {
	return strlen(string) == 0;
}