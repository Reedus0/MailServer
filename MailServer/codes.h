#pragma once

#define SERVICE_READY "220 Hello, from server!\r\n"
#define SERVICE_CLOSING_TRANSMISSION "221 Bye!\r\n"
#define ACTION_OK "250 Action ok.\r\n"

#define START_MAIL_INPUT "354 End mail with single .\r\n"

#define SYNTAX_ERROR "500 Syntax error!\r\n"
#define SYNTAX_ERROR_PARAMETERS "501 Syntax error in parameters!\r\n"
#define BAD_SEQUENCE "503 Bad sequence of commands!\r\n"
#define USER_NOT_LOCAL "551 User not local!\r\n"
#define TRANSACTION_FAILED "554 Transaction failed!\r\n"