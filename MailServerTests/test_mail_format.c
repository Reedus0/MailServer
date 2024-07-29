#include <stdio.h>
#include "smtp_request.h"
#include "mail.h"
#include "func.h"
#include "make.h"
#include "mail_format.h"

int test_has_to_header_with_two_recipients() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail\r\nDate: now\r\n\r\nMail text\r\n");
    struct mail* mail = init_mail();

    smtp_request_add_recipient(smtp_request, string_to_email_address("carl@domain.local"));

    format_mail(mail, smtp_request);

    if (string_except_eq(mail_get_header_value(mail, "To"), "carl@domain.local, john@domain.local")) {
        printf("test_has_to_header_with_two_recipients OK\n");
        return 1;
    }
    return 0;
}

int test_has_header() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail\r\nDate: now\r\nTo: John Camel\r\n\r\nMail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail_get_header_value(mail, "To"), "John Camel")) {
        printf("test_has_header OK\n");
        return 1;
    }
    return 0;
}

int test_space_headers() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", " Subject: Mail\r\n\r\nMail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, " Subject: Mail\r\n\r\nMail text\r\n")) {
        printf("test_space_headers OK\n");
        return 1;
    }
    return 0;
}

int test_no_headers() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Mail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\r\n")) {
        printf("test_no_headers OK\n");
        return 1;
    }
    return 0;
}

int test_pre_enter() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "\r\nSubject: Mail subject\r\nMail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "\r\nSubject: Mail subject\r\nMail text\r\n")) {
        printf("test_pre_enter OK\n");
        return 1;
    }
    return 0;
}

int test_no_double_enter() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail subject\r\nMail text\r\nData: now\r\n\r\nMail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\r\nData: now\r\n\r\nMail text\r\n")) {
        printf("test_no_double_enter OK\n");
        return 1;
    }
    return 0;
}

int test_emptry_message_two_enter() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail subject\r\nDate: now\r\n\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "")) {
        printf("test_emptry_message_two_enter OK\n");
        return 1;
    }
    return 0;
}

int test_emptry_message_one_enter() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail subject\r\nDate: now\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "")) {
        printf("test_emptry_message_one_enter OK\n");
        return 1;
    }
    return 0;
}

int test_normal() {
    struct smtp_request* smtp_request = make_smtp_request("host.domain.local", "john@domain.local", "john@domain.local", "Subject: Mail subject\r\nDate: now\r\n\r\nMail text\r\n");
    struct mail* mail = init_mail();

    format_mail(mail, smtp_request);

    if (string_except_eq(mail->text, "Mail text\r\n")) {
        printf("test_normal OK\n");
        return 1;
    }
    return 0;
}