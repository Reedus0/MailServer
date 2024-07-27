#include <stdio.h>
#include "email_address.h"
#include "mail.h"
#include "func.h"
#include "make.h"
#include "format.h"

int test_make_email_string_normal() {
    struct email_address* email_address = make_email_address("john@domain.local");

    char* result = email_address_to_string(email_address);

    if (string_except_eq(result, "john@domain.local")) {
        printf("test_make_email_string_normal OK\n");
        return 1;
    }
    return 0;
}

int test_make_email_arrows() {
    struct email_address* email_address = make_email_address("<john@domain.local>");

    if (string_except_eq(email_address->user, "john") && string_except_eq(email_address->domain, "domain.local")) {
        printf("test_make_email_arrows OK\n");
        return 1;
    }
    return 0;
}

int test_make_email_normal() {
    struct email_address* email_address = make_email_address("john@domain.local");

    if (string_except_eq(email_address->user, "john") && string_except_eq(email_address->domain, "domain.local")) {
        printf("test_make_email_normal OK\n");
        return 1;
    }
    return 0;
}