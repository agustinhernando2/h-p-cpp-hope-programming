#pragma once
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cjson_handler.h>

#define LAST_KEEP_ALIVED "last_keepalived"
#define LAST_EVENT "last_event"
#define SIZE_TIME 20
#define K_EMERGENCY "emergency"
#define LOG_E_FILE_PATH "data/emergency.log"
#define JSON_FILE_PATH "data/state_summary.json"

int run_emergency_handler(int *pipe_fd);

int add_event_to_json(char *key, char *value);
