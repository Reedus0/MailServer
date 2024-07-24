#pragma once

#define SERVICE_READY "220 Hello, from server!\n"
#define SERVICE_CLOSING_TRANSMISSION "221 Bye!\n"
#define ACTION_OK "250 Action ok.\n"

#define START_MAIL_INPUT "354 End mail with single .\n"

#define SYNTAX_ERROR "500 Syntax error!\n"
#define SYNTAX_ERROR_PARAMETERS "501 Syntax error in parameters!\n"
#define BAD_SEQUENCE "503 Bad sequence of commands!\n"
#define USER_NOT_LOCAL "551 User not local!\n"
#define TRANSACTION_FAILED "554 Transaction failed!\n"