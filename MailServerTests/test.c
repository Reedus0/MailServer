#include <stdio.h>
#include <string.h>
#include "header.h"

#include "test_format.h"
#include "test_email_address.h"

int test_list(char* list_name, int (*tests[])()) {
    printf("\nStarting %s tests...\n\n", list_name);
    int i = 0;
    int result = 0;
    while (tests[i] != NULL) {
        result += tests[i]();
        i += 1;
    }
    printf("\nTest count: %d, passed: %d\n", i, result);
}

int main(int argv, char* argc[]) {
    int (*email_address_tests[])() = {
        test_make_email_normal,
        test_make_email_arrows,
        test_make_email_string_normal,
        NULL
    };
    test_list("email_address", email_address_tests);

    int (*format_tests[])() = {
        test_normal,
        test_emptry_message_one_enter,
        test_emptry_message_two_enter,
        test_no_double_enter,
        test_pre_enter,
        test_no_headers,
        test_space_headers,

        test_header_count,
        NULL
    };
    test_list("format", format_tests);

    return 0;
}