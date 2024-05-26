#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cjson_handler.h>

#define NORT_ENTRY "NORTH"
#define WEST_ENTRY "WEST"
#define EAST_ENTRY "EAST"
#define SOUTH_ENTRY "SOUTH"

// temperature range
#define MIN_TEMPERATURE 40
#define MAX_TEMPERATURE 50
#define HIGH_TEMPERATURE 38
#define BUFFER_SIZE_QMS 512
#define BUFFER_SIZE_M 100

#define K_Q_MESSAGE "message"
#define K_Q_COMMAND "command"
#define ALERT "1"
#define K_ALERTS "alerts"

#define SIZE_TIME 20
// log file
#define LOG_FILE_PATH "data/temperature.log"
#define JSON_FILE_PATH "data/state_summary.json"

// message queue
#define K_MSG "build"

#ifdef __cplusplus
extern "C" {
#endif

extern int msg_id;
typedef struct
{
    long mtype;
    char message[BUFFER_SIZE_M];
} mess_t;

typedef struct
{
    char timestamp[SIZE_TIME];
    char location[SIZE_TIME];
    double temperature;
} str_temp;

/*
* @brief Initializes the message queue
*/
int create_message_queue();

/**
 * @brief Get the temperature
*/
double get_temperature();

/**
 * @brief Simulate each temperature sensor
*/
void simulate_temperature_sensor(char *entry);

/**
 * @brief Run the alert module
*/
void run_alert_module();

/**
 * @brief Send the alert to the message queue
*/
void send_alert_msqueue(mess_t *send_buffer);

#ifdef __cplusplus
}
#endif