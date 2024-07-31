#include <string.h>
#include "status.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a > b ? b : a)

int string_except_eq(char* first, char* second) {
	return !memcmp(first, second, MAX(strlen(first), strlen(second)));
}

int string_except_not_eq(char* first, char* second) {
	return !string_except_eq(first, second);
}

int int_except_eq(int first, int second) {
	return first == second;
}

int int_except_not_eq(int first, int second) {
	return !int_except_eq(first, second);
}

int ptr_except_eq(void* first, void* second) {
	return first == second;
}

int ptr_except_not_eq(void* first, void* second) {
	return !ptr_except_eq(first, second);
}

int status_except_ok(enum STATUS first) {
	return first == STATUS_OK;
}

int status_except_not_ok(enum STATUS first) {
	return first == STATUS_NOT_OK;
}