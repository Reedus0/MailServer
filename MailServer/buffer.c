#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdarg.h>
#include <ctype.h>

void add_to_buffer(char* buffer, char* message) {
	memcpy(buffer + strlen(buffer), message, strlen(message) + 1);
}

void flush_to_buffer(char* buffer, int size, char* format, ...) {
	va_list va;
	va_start(va, format);
	vsnprintf(buffer + strlen(buffer), size, format, va);
	va_end(va);
}

char* copy_buffer(char* buffer) {
	char* result = calloc(strlen(buffer) + 1, sizeof(char));
	memcpy(result, buffer, strlen(buffer));
	return result;
}

char* get_field_from_buffer(char* message, int offset) {
	char* result = calloc(offset + 1, sizeof(char));
	memcpy(result, message, offset);
	return result;
}

char* get_value_from_buffer(char* message, int offset) {
	int new_length = strlen(message) - offset - 1;
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, message + offset + 1, new_length - 1);
	return result;
}

int buffer_has_command(char* command, char* message) {
	return !memcmp(message, command, strlen(command));
}

static char* ltrim(char* string) {
	while (isspace(*string)) string++;
	return string;
}

static char* rtrim(char* string) {
	char* back = string + strlen(string);
	while (isspace(*--back));
	*(back + 1) = '\0';
	return string;
}

char* trim_string(char* string) {
	char* left_trim = ltrim(string);
	if (is_empty_string(left_trim)) {
		return left_trim;
	}
	return rtrim(left_trim);
}

int is_empty_string(char* string) {
	return strlen(string) == 0;
}