#pragma once

enum STATUS serve_rcpt_to(SOCKET sock, char* buffer, struct smtp_request* smtp_request, enum server_states current_state);