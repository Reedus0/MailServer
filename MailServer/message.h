#pragma once

void add_to_message(char* buffer, char* message);
char* get_field_from_message(char* message, int offset);
char* get_value_from_message(char* message, int offset);
int check_message_command(char* command, char* message);
char* trim_string(char* string);
char* is_empty_string(char* string);