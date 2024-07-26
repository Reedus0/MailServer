#include "stdio.h"
#include "smtp_request.h"
#include "mail.h"
#include "func.h"
#include "make.h"
#include "format.h"

static int test_header_count() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "Subject: Mail subject\nDate: now\n\nMail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (int_except_eq(mail->headers_count, 2)) {
        printf("test_header_count OK\n");
        return 1;
    }
    return 0;
}

static int test_space_headers() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", " Subject: Mail\n\nMail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, " Subject: Mail\n\nMail text\n")) {
        printf("test_space_headers OK\n");
        return 1;
    }
    return 0;
}

static int test_no_headers() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "Mail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\n")) {
        printf("test_no_headers OK\n");
        return 1;
    }
    return 0;
}

static int test_pre_enter() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "\nSubject: Mail subject\nMail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "\nSubject: Mail subject\nMail text\n")) {
        printf("test_pre_enter OK\n");
        return 1;
    }
    return 0;
}

static int test_no_double_enter() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "Subject: Mail subject\nMail text\nData: now\n\nMail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\nData: now\n\nMail text\n")) {
        printf("test_not_double_enter OK\n");
        return 1;
    }
    return 0;
}

static int test_normal() {
    struct smtp_request* smtp_request = make_smtp_request("", "john@domain.local", "john@domain.local", "Subject: Mail subject\nDate: now\n\nMail text\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\n")) {
        printf("test_normal OK\n");
        return 1;
    }
    return 0;
}

int test_format() {
    printf("Start test_format\n");
    test_normal();
    test_no_double_enter();
    test_pre_enter();
    test_no_headers();
    test_space_headers();

    test_header_count();
}