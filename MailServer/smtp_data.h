#pragma once

#define SMTP_REQUEST_MAIL_SIZE 8192

enum STATUS serve_data(SOCKET sock, char* buffer, struct smtp_request* smtp_request, enum server_states current_state);