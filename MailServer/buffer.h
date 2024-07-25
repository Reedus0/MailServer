#pragma once

void add_to_buffer(char* buffer, char* message);
void flush_to_buffer(char* buffer, int size, char* format, ...);
char* copy_buffer(char* buffer);
char* get_field_from_buffer(char* message, int offset);
char* get_value_from_buffer(char* message, int offset);
int buffer_has_command(char* command, char* message);
char* trim_string(char* string);
char* is_empty_string(char* string);