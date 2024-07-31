#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include "header.h"

#include "test_mail_format.h"
#include "test_email_address.h"
#include "test_list.h"
#pragma comment(lib, "ws2_32.lib")

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

    WSADATA wsa_data;

    int status = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (status != 0) {
        return 1;
    }

    char* config = "domain = domain.local";

    config_parse_buffer(config);

    int (*list_tests[])() = {
        test_list_insert,
        test_list_remove_last,
        test_list_remove_center,
        NULL
    };
    test_list("list", list_tests);

    int (*email_address_tests[])() = {
        test_make_email_normal,
        test_make_email_arrows,
        test_make_email_string_normal,
        test_validate_email_string_no_domain,
        test_validate_email_string_no_user,
        test_validate_email_two_email_symbols,
        test_validate_email_no_email_symbol,
        NULL
    };
    test_list("email_address", email_address_tests);

    int (*format_tests[])() = {
        test_normal,
        test_empty_message_one_enter,
        test_empty_message_two_enter,
        test_no_double_enter,
        test_pre_enter,
        test_no_headers,
        test_space_headers,
        test_has_header,
        test_has_header_with_two_recipients,
        test_has_received_header,
        test_x_original_to_replace,
        test_has_timestamp,
        NULL
    };
    test_list("format", format_tests);

    return STATUS_NOT_OK;
}