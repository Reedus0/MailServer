#pragma once

int string_except_eq(char* first, char* second);
int string_except_not_eq(char* first, char* second);
int int_except_eq(int first, int second);
int int_except_not_eq(int first, int second);
int ptr_except_eq(void* first, void* second);
int ptr_except_not_eq(void* first, void* second);
int status_except_ok(enum STATUS first);
int status_except_not_ok(enum STATUS first);