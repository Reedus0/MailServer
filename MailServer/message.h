#pragma once

void add_to_message(char* buffer, char* message);
char* get_field_from_message(char* message, int offset);
char* get_value_from_message(char* message, int offset);
int check_message_command(char* command, char* message);
char* get_email_user(char* email);
char* get_email_domain(char* email);
int validate_email(char* email);