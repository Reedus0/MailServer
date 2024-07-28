#pragma once

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a > b ? b : a)

#define MAX_PENDING_CONNECTIONS 8

enum STATUS {
	STATUS_OK = 1,
	STATUS_ERROR = -1,
	STATUS_NOT_OK = 0
};