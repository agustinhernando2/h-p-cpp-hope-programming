#include <emergency_handler.h>

int run_emergency_handler(int *pipe_fd)
{
    char timestamp[SIZE_TIME];
    char message_[] = "Last keepalived signal received.";

    set_timestamp(timestamp, SIZE_TIME);
    generate_log(LOG_E_FILE_PATH, timestamp, message_);
    add_event_to_json(LAST_KEEP_ALIVED, timestamp);
    // close read end
    close(pipe_fd[0]);
    sleep(60);
    fprintf(stdout, "WARNING !!! 5sec to close the connections\n");
    sleep(5);
    // write to pipe
    char message[] = "Server failure. Emergency notification sent to all connected clients.";

    generate_log(LOG_E_FILE_PATH, timestamp, message);

    char event[512];
    strcat(event, timestamp);
    strcat(event, ", ");
    strcat(event, message);

    add_event_to_json(LAST_EVENT, event);

    if (write(pipe_fd[1], message, sizeof(message)) == -1)
    {
        perror("write");
        return 1;
    }
    // send SIGINT to parent
    kill(getppid(), SIGINT);

    // Close write end
    close(pipe_fd[1]);

    return 0;
}

int add_event_to_json(char *key, char *value)
{
    char *obj_of_json_buffer = NULL;
    char *json_buffer = NULL;

    if (read_file(JSON_FILE_PATH, &json_buffer))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // get the value of the key alerts, ex {"alerts":{...}}
    if (get_value_of_key_from_json_string(json_buffer, K_EMERGENCY, &obj_of_json_buffer))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // update the value of the key
    if (cjson_add_key_value_to_json_string(obj_of_json_buffer, key, value, OVERRIDE | INTPARSE))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // update the object
    if (cjson_add_key_value_to_json_string(json_buffer, K_EMERGENCY, obj_of_json_buffer, OVERRIDE | OBJPARSE))
    {
        error_handler("Error ", __FILE__, __LINE__);
        return 1;
    }
    // write the updated supplies object to the file
    return write_file(JSON_FILE_PATH, json_buffer);
}
