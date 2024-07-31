#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdarg.h>
#include <ctype.h>

void add_to_buffer(char* buffer, char* message) {
	memcpy(buffer + strlen(buffer), message, strlen(message) + 1);
}

void flush_to_buffer(char* buffer, int count, char* format, ...) {
	va_list va;
	va_list args;
	va_start(args, format);
	int total_size = strlen(format) - (2 * count) + 1;
	for (int i = 0; i < count; i++) {
		total_size += strlen(va_arg(args, char*));
	}
	va_start(va, format);
	vsnprintf(buffer + strlen(buffer), total_size, format, va);
	va_end(args);
	va_end(va);
}

void lower_buffer(char* string) {
	while (*string != '\0') {
		*string = tolower(*string);
		string++;
	}
	return string;
}

char* copy_buffer(char* buffer) {
	char* result = calloc(strlen(buffer) + 1, sizeof(char));
	memcpy(result, buffer, strlen(buffer));
	return result;
}

static char* ltrim(char* string) {
	char* char_pointer = string;
	while (isspace(*char_pointer)) char_pointer++;
	memcpy(string, char_pointer, strlen(char_pointer) + 1);
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

char* get_field_from_buffer(char* message, char* separator) {
	char* separator_pointer = strstr(message, separator);
	int new_length = separator_pointer - message;
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, message, new_length);
	return result;
}

char* get_value_from_buffer(char* message, char* separator) {
	char* separator_pointer = strstr(message, separator) + strlen(separator);
	int new_length = strlen(separator_pointer);
	char* result = calloc(new_length + 1, sizeof(char));
	memcpy(result, separator_pointer, new_length);
	return result;
}

char* buffer_get_next(char** iterator, char* separator) {
	char* start_of_line = *iterator;
	char* end_of_line = strstr(*iterator, separator);

	int result_length = end_of_line - *iterator;

	if (end_of_line == NULL) {
		result_length = strlen(*iterator);
	}

	char* result = calloc(result_length + 1, sizeof(char));
	memcpy(result, *iterator, result_length);
 	result = trim_string(result);
	*iterator = end_of_line + strlen(separator);

	if (end_of_line == NULL) {
		*iterator = NULL;
	}
	
	return result;
}

int buffer_has_command(char* command, char* message) {
	return !memcmp(message, command, strlen(command)) != NULL;
}

int is_empty_string(char* string) {
	return strlen(string) == 0;
}

int compare_strings(char* first, char* second) {
	if (strlen(first) != strlen(second)) {
		return 0;
	}
	if (memcmp(first, second, strlen(first))) {
		return 0;
	}
	return 1;
}