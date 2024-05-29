
#include <alert_module.h>

void run_alert_module()
{
    create_message_queue();
    while (1)
    {
        sleep(5);
        simulate_temperature_sensor(NORT_ENTRY);
        sleep(2);
        simulate_temperature_sensor(EAST_ENTRY);
        sleep(1);
        simulate_temperature_sensor(WEST_ENTRY);
        sleep(4);
        simulate_temperature_sensor(SOUTH_ENTRY);
    }
}

void simulate_temperature_sensor(char* entry)
{
    str_temp str_temp;
    mess_t buffer;
    memset(&str_temp, 0, sizeof(str_temp));

    str_temp.temperature = get_temperature();
    strcpy(str_temp.location, entry);
    set_timestamp(str_temp.timestamp, SIZE_TIME);

    char message[BUFFER_SIZE_M];

    // Generate log with timestamp and temperature
    sprintf(message, "%s ENTRY, %.1f °C\n", str_temp.location, str_temp.temperature);

    if (str_temp.temperature > HIGH_TEMPERATURE)
    {

        memset(message, 0, BUFFER_SIZE_M);
        sprintf(message, "{\"location\":\"%s\",\"temperature\":%.1f}", str_temp.location, str_temp.temperature);
        buffer.mtype = ALERT;
        strcpy(buffer.message, message);

        send_alert_msqueue(&buffer);
    }
}

double get_temperature()
{
    // Simulate temperature
    srand((unsigned int)time(NULL));
    // return a number between MIN_TEMPERATURE and MAX_TEMPERATURE
    return ((rand() / 10) % (MAX_TEMPERATURE - MIN_TEMPERATURE)) + MIN_TEMPERATURE;
}