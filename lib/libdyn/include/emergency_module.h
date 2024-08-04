#pragma once
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lib_handler.h>
#include <msg_handler.h>

#define LAST_KEEP_ALIVED "last_keepalived"
#define LAST_EVENT "last_event"
#define K_EMERGENCY "emergency"
#define SIZE_TIME 20
#define EMERGENCY 2

#ifdef __cplusplus
extern "C"
{
#endif

    extern int msg_id;

    /**
     * @brief This function is used to handle the emergency module
     */
    void run_emergency_module();

#ifdef __cplusplus
}
#endif
