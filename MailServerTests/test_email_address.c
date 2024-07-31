#include <stdio.h>
#include "email_address.h"
#include "mail.h"
#include "func.h"
#include "make.h"
#include "mail_format.h"

int test_validate_email_no_email_symbol() {
    enum STATUS result = validate_email_string("john_domain.local");

    if (status_except_not_ok(result)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_validate_email_two_email_symbols() {
    enum STATUS result = validate_email_string("john@domain.local@domain.local");

    if (status_except_not_ok(result)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_validate_email_string_no_user() {
    enum STATUS result = validate_email_string("john@");

    if (status_except_not_ok(result)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_validate_email_string_no_domain() {
    enum STATUS result = validate_email_string("@domain.local");

    if (status_except_not_ok(result)) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_validate_email_arrows() {
    struct email_address* email_address = make_email_address("<john@domain.local>");

    if (string_except_eq(email_address->user, "john") && string_except_eq(email_address->domain, "domain.local")) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_make_email_string_normal() {
    struct email_address* email_address = make_email_address("john@domain.local");

    char* result = email_address_to_string(email_address);

    if (string_except_eq(result, "john@domain.local")) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_make_email_arrows() {
    struct email_address* email_address = make_email_address("<john@domain.local>");

    if (string_except_eq(email_address->user, "john") && string_except_eq(email_address->domain, "domain.local")) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}

int test_make_email_normal() {
    struct email_address* email_address = make_email_address("john@domain.local");

    if (string_except_eq(email_address->user, "john") && string_except_eq(email_address->domain, "domain.local")) {
        printf("%s OK\n", __func__);
        return 1;
    }
    printf("%s NOT OK\n", __func__);
    return 0;
}