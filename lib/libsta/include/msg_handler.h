#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <lib_handler.h>

#define BUFFER_SIZE_M 512

// message queue
#define K_MSG "build/"

#ifdef __cplusplus
extern "C"
{
#endif

    extern int msg_id;
    /**
     * @brief Structure to store the message
     */
    typedef struct
    {
        /**
         * @brief Message type
         */
        long mtype;
        /**
         * @brief Message
         */
        char message[BUFFER_SIZE_M];
    } mess_t;

    /**
     * @brief Create the message queue
     */
    int create_message_queue();

    /**
     * @brief Send the alert to the message queue
     */
    void send_alert_msqueue(mess_t* send_buffer);

#ifdef __cplusplus
}
#endif
