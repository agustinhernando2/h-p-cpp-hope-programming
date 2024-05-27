#include <emergency_module.h>

void run_emergency_module()
{   
    mess_t buffer;
    char last_keepalived[SIZE_TIME];
    char message_[] = "Last keepalived signal received.";

    set_timestamp(last_keepalived, SIZE_TIME);
    generate_log(LOG_E_FILE_PATH, last_keepalived, message_);
    sleep(360);
    fprintf(stdout, "WARNING !!! 10 sec to close the connections\n");
    sleep(10);
    char last_event[] = "Server failure. Emergency notification sent to all connected clients.";

    generate_log(LOG_E_FILE_PATH, last_keepalived, last_event);

    char message[BUFFER_SIZE_M];
    memset(message, 0, BUFFER_SIZE_M);
    sprintf(message, "{\"last_keepalived\":\"%s\",\"last_event\":\"%s\"}", last_keepalived, last_event);

    // send to message queue
    buffer.mtype = EMERGENCY;
    strcpy(buffer.message, message);

    send_alert_msqueue(&buffer);
}
