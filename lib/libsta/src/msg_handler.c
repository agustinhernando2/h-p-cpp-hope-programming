
#include <msg_handler.h>

void send_alert_msqueue(mess_t* send_buffer)
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

int create_message_queue()
{
    // add random number to the key
    int random = time(NULL);
    char key_path[100];
    sprintf(key_path, "%s_%d", K_MSG, random);

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
        error_handler(". error", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    printf("Message queue created QID = %d, using key = %s\n", msg_id, key_path);

    return msg_id;
}
