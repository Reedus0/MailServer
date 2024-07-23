#include <string.h>
#include <stdarg.h>
#include "codes.h"

void add_to_message(char* buffer, char* message) {
	memcpy(buffer + strlen(buffer), message, strlen(message) + 1);
}