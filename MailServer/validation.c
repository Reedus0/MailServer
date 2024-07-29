#include <string.h>
#include "header.h"
#include "buffer.h"
#include "net.h"

enum STATUS validate_without_args(char* request, char* command) {
	int message_length = get_message_length(request);

	if (message_length != strlen(command)) {
		return STATUS_NOT_OK;
	}

	return STATUS_OK;
}

enum STATUS validate_with_args(char* request, char* command, char* separator) {
	int message_length = get_message_length(request);

	if (message_length == strlen(command)) {
		return STATUS_NOT_OK;
	}

	char* buffer = get_value_from_buffer(request, separator);
	buffer = trim_string(buffer);

	if (is_empty_string(buffer)) {
		return STATUS_NOT_OK;
	}

	free(buffer);

	return STATUS_OK;
}