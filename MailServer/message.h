#pragma once

void add_to_message(char* buffer, char* code);
char* get_value_from_message(char* message, int offset);
int check_message_command(char* command, char* message);