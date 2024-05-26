
#include <alert_module.h>

void run_alert_module()
{
    create_message_queue();
    while (1)
    {
        sleep(10);
        simulate_temperature_sensor(NORT_ENTRY);
        simulate_temperature_sensor(EAST_ENTRY);
        simulate_temperature_sensor(WEST_ENTRY);
        simulate_temperature_sensor(SOUTH_ENTRY);
    }
}

void simulate_temperature_sensor(char *entry)
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
    generate_log(LOG_FILE_PATH, str_temp.timestamp, message);

    if (str_temp.temperature > HIGH_TEMPERATURE)
    {

        memset(message, 0, BUFFER_SIZE_M);
        sprintf(message, "{\"location\":\"%s\",\"temperature\":%.1f}", str_temp.location, str_temp.temperature);
        buffer.mtype = 1;
        strcpy(buffer.message, message);

        send_alert_msqueue(&buffer);
    }
}

void send_alert_msqueue(mess_t *send_buffer)
{
    if (msg_id == -1 || msg_id == 0)
    {
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    if (msgsnd(msg_id, send_buffer, sizeof(send_buffer->message), 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
}

double get_temperature()
{
    // Simulate temperature
    srand((unsigned int)time(NULL));
    // return a number between MIN_TEMPERATURE and MAX_TEMPERATURE
    return ((rand() / 10) % (MAX_TEMPERATURE - MIN_TEMPERATURE)) + MIN_TEMPERATURE;
}
// int add_entry_to_json(char *entry)
// {
//     char *obj_of_json_buffer = NULL;
//     char *value_char = NULL;
//     char *json_buffer = NULL;

//     if (read_file(JSON_FILE_PATH, &json_buffer))
//     {
//         error_handler("Error ", __FILE__, __LINE__);
//         return 1;
//     }
//     // get the value of the key alerts, ex {"alerts":{...}}
//     if (get_value_of_key_from_json_string(json_buffer, K_ALERTS, &obj_of_json_buffer))
//     {
//         error_handler("Error ", __FILE__, __LINE__);
//         return 1;
//     }
//     // get the value of the key category, ex {"north_entry":{...}}
//     if (get_value_of_key_from_json_string(obj_of_json_buffer, entry, &value_char))
//     {
//         error_handler("Error ", __FILE__, __LINE__);
//         return 1;
//     }
//     int value = atoi(value_char);
//     value++;
//     sprintf(value_char, "%d", value);
//     // update the value of the key
//     if (cjson_add_key_value_to_json_string(obj_of_json_buffer, entry, value_char, OVERRIDE | INTPARSE))
//     {
//         error_handler("Error ", __FILE__, __LINE__);
//         return 1;
//     }
//     // update the object
//     if (cjson_add_key_value_to_json_string(json_buffer, K_ALERTS, obj_of_json_buffer, OVERRIDE | OBJPARSE))
//     {
//         error_handler("Error ", __FILE__, __LINE__);
//         return 1;
//     }
//     // write the updated supplies object to the file
//     return write_file(JSON_FILE_PATH, json_buffer);
// }

int create_message_queue()
{
    // Create a unique key
    key_t key = ftok(K_MSG, 'm');
    if (key == -1)
    {
        error_handler("Error ftok", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    msg_id = msgget(key, IPC_CREAT | 0666);
    if (msg_id == -1 || msg_id == 0)
    {
        error_handler("msgget error", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("Message queue created QID = %d\n", msg_id);

    return 0;
}