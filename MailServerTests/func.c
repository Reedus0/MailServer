#include <string.h>

#define MAX(a,b) (a > b ? a : b)

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