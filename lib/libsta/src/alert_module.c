
#include <alert_module.h>

double get_temperature()
{
    // Simulate temperature
    srand((unsigned int)time(NULL));
    // return a number between MIN_TEMPERATURE and MAX_TEMPERATURE
    return ((rand() / 10) % (MAX_TEMPERATURE - MIN_TEMPERATURE)) + MIN_TEMPERATURE;
}

void simulate_temperature_sensor(char *entry)
{
    str_temp str_temp;
    memset(&str_temp, 0, sizeof(str_temp));

    str_temp.temperature = get_temperature();
    strcpy(str_temp.location, entry);
    set_timestamp(str_temp.timestamp, SIZE_TIME);

    char message[BUFFER_SIZE_M];
    char send_buffer[BUFFER_SIZE_QMS];

    memset(message, 0, BUFFER_SIZE_M);
    memset(send_buffer, 0, BUFFER_SIZE_QMS);

    sprintf(message, "%s ENTRY, %.1f °C\n", str_temp.location, str_temp.temperature);
    generate_log(LOG_FILE_PATH, str_temp.timestamp, message);

    if (str_temp.temperature > HIGH_TEMPERATURE)
    {
        strcat(message, "\n¡¡¡ ALERTA !!!\n");
        memset(send_buffer, 0, BUFFER_SIZE_QMS);
        cjson_add_key_value_to_json_string(send_buffer, K_Q_MESSAGE, message, OVERRIDE);
        cjson_add_key_value_to_json_string(send_buffer, K_Q_COMMAND, ALERT, OVERRIDE);
        send_alert_msqueue(message);
        add_entry_to_json(entry);
    }
}

void run_alert_module()
{
    while (1)
    {
        sleep(2);
        simulate_temperature_sensor(NORT_ENTRY);
        simulate_temperature_sensor(EAST_ENTRY);
        simulate_temperature_sensor(WEST_ENTRY);
        simulate_temperature_sensor(SOUTH_ENTRY);
    }
}

void send_alert_msqueue(char *message)
{
    mess_t send_buffer;
    send_buffer.mtype = 1;
    strcpy(send_buffer.message, message);

    if (msg_id == -1 || msg_id == 0)
    {
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    // fprintf(stdout, "mns: %s\n", send_buffer);
    if (msgsnd(msg_id, &send_buffer, sizeof(send_buffer), 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
}

int add_entry_to_json(char *entry)
{
    char *obj_of_json_buffer = NULL;
    char *value_char = NULL;
    char *json_buffer = NULL;

    if (read_file(JSON_FILE_PATH, &json_buffer))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // get the value of the key alerts, ex {"alerts":{...}}
    if (get_value_of_key_from_json_string(json_buffer, K_ALERTS, &obj_of_json_buffer))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // get the value of the key category, ex {"north_entry":{...}}
    if (get_value_of_key_from_json_string(obj_of_json_buffer, entry, &value_char))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    int value = atoi(value_char);
    value++;
    sprintf(value_char, "%d", value);
    // update the value of the key
    if (cjson_add_key_value_to_json_string(obj_of_json_buffer, entry, value_char, OVERRIDE | INTPARSE))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // update the object
    if (cjson_add_key_value_to_json_string(json_buffer, K_ALERTS, obj_of_json_buffer, OVERRIDE | OBJPARSE))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // write the updated supplies object to the file
    return write_file(JSON_FILE_PATH, json_buffer);
}
