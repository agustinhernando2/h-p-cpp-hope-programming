#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <msg_handler.h>
#include <cjson_handler.h>

#define NORT_ENTRY "NORTH"
#define WEST_ENTRY "WEST"
#define EAST_ENTRY "EAST"
#define SOUTH_ENTRY "SOUTH"

// temperature range
#define MIN_TEMPERATURE 35
#define MAX_TEMPERATURE 42
#define HIGH_TEMPERATURE 38
#define BUFFER_SIZE_QMS 512

#define K_Q_MESSAGE "message"
#define K_Q_COMMAND "command"
#define ALERT 1
#define K_ALERTS "alerts"

#define SIZE_TIME 20
// log file
#define LOG_FILE_PATH "data/temperature.log"
#define JSON_FILE_PATH "data/state_summary.json"




typedef struct
{
    char timestamp[SIZE_TIME];
    char location[SIZE_TIME];
    double temperature;
} str_temp;

/**
 * @brief Get the temperature
*/
double get_temperature();

/**
 * @brief Simulate each temperature sensor
*/
void simulate_temperature_sensor(char *entry);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run the alert module
*/
void run_alert_module();

#ifdef __cplusplus
}
#endif