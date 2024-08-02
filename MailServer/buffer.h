#pragma once

void add_to_buffer(char* buffer, char* message);
void flush_to_buffer(char* buffer, int count, char* format, ...);
void lower_buffer(char* string);
char* copy_buffer(char* buffer);
char* get_field_from_buffer(char* message, char* separator);
char* get_value_from_buffer(char* message, char* separator);
char* buffer_get_next(char** iterator, char* separator);
int buffer_has_command(char* command, char* message);
char* trim_string(char* string);
int is_empty_string(char* string);
int compare_strings(char* first, char* second);