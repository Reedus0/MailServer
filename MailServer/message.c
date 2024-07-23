#include <string.h>
#include <stdarg.h>
#include "codes.h"

void add_to_message(char* message, char* new_part) {
	memcpy(message + strlen(message), new_part, strlen(new_part) + 1);
}

void build_message(char* buffer, char* code, ...) {
	va_list argptr;
	va_start(argptr, code);
	add_to_message(buffer, code);
	while (1) {
		char* next_string = va_arg(argptr, char*);
		add_to_message(buffer, next_string);
		if (next_string == "\n") {
			break;
		}
	}
	va_end(argptr);
}