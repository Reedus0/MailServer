#include <stdio.h>
#include <string.h>
#include "header.h"

#include "test_format.h"
#include "test_email_address.h"
#include "test_list.h"
#include "test_smtp_request.h"

int test_list(char* list_name, int (*tests[])()) {
    printf("\nStarting %s tests...\n\n", list_name);
    int i = 0;
    int result = 0;
    while (tests[i] != NULL) {
        int test_result = tests[i]();
        result += test_result;
        i += 1;
    }
    printf("\nTest count: %d, passed: %d\n", i, result);
}

int main(int argv, char* argc[]) {

    int (*list_tests[])() = {
        test_list_insert,
        test_list_remove_last,
        test_list_remove_center,
        NULL
    };
    test_list("list", list_tests);

    int (*smtp_request_tests[])() = {
        test_add_multiple_recipients,
        NULL
    };
    test_list("smtp_request", smtp_request_tests);

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
        NULL
    };
    test_list("format", format_tests);

    return 0;
}