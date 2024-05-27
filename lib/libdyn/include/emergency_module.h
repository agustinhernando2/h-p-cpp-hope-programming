#pragma once
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <msg_handler.h>
#include <lib_handler.h>

#define LAST_KEEP_ALIVED "last_keepalived"
#define LAST_EVENT "last_event"
#define K_EMERGENCY "emergency"
#define SIZE_TIME 20
#define EMERGENCY 2
#define LOG_E_FILE_PATH "data/emergency.log"


#ifdef __cplusplus
extern "C" {
#endif

extern int msg_id;

void run_emergency_module();

#ifdef __cplusplus
}
#endif