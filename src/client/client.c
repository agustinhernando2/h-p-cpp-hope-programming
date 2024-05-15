#include <client.h>

int main(int argc, char *argv[])
{
    set_signal_handlers();

    if (argc < 3)
    {
        fprintf(stderr, "Uso %s <host> IPV\n", argv[0]);
        fprintf(stderr, "Example %s localhost 4\n", argv[0]);
        fprintf(stderr, "Example %s localhost 6\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    server = gethostbyname(argv[1]);

    get_credentials();

    if (check_credentials())
    {
        // if the user is admin, the connection will be TCP
        connection_type = SOCK_STREAM;
    }
    else
    {
        // if the user is not admin, the connection will be UDP
        fprintf(stdout, "Welcome Normal user\n");
        connection_type = SOCK_DGRAM;
        flag_get_supply = 1;
        skip_option = 0;
    }

    ip_version = atoi(argv[2]);

    try_connect_server(connection_type, ip_version);

    while (TRUE)
    {
        if (skip_option == 1)
        {
            memset(send_socket_buffer, 0, BUFFER_SIZE);
            get_options();
            add_credentials();
        }
        else
        {
            // add command to receive the supplies
            memset(send_socket_buffer, 0, BUFFER_SIZE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND, OPTION1, OVERRIDE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND_EQ, OPTION1_EQ, OVERRIDE);
        }
        skip_option = 1;
        send_message_to_server();
        recv_and_check_message();
    }
    return 0;
}

void send_message_to_server()
{
    if (send_message(send_socket_buffer, BUFFER_SIZE, sockfd))
    {
        error_handler("Error send_message", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

int check_credentials()
{
    if (strcmp(username, ADMIN) == 0 && strcmp(password, ADMIN) == 0)
    {
        fprintf(stdout, "Welcome Admin\n");
        sleep(1);
        return 1;
    }
    return 0;
}

void try_connect_server(int type, int ipv)
{
    switch (ipv)
    {
    case IPV4:
        if (connect_client_ipv4(&sockfd, IP_ADDR_V4, type))
        {
            error_handler("Error to connect to the server", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        break;
    case IPV6:
        if (connect_client_ipv6(&sockfd, IP_ADDR_V6, type))
        {
            error_handler("Error to connect to the server", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        break;
    default:
        fprintf(stderr, "Uso <host> IPV\n");
        fprintf(stderr, "Example client localhost 4\n");
        fprintf(stderr, "Example client localhost 6\n");
        exit(EXIT_SUCCESS);
        break;
    }
}

int recv_and_check_message()
{
    memset(recv_socket_buffer, 0, BUFFER_SIZE);
    recv_tcp_message(sockfd, recv_socket_buffer);

    if (is_key_in_json_buffer(recv_socket_buffer, K_ACC_DENEID))
    {
        clear_screen();
        // print message
        fprintf(stdout, "Access denied\n");
        sleep(2);
    }
    if (is_key_in_json_buffer(recv_socket_buffer, K_END))
    {
        end_client_conn();
    }
    // show message
    clear_screen();
    print_cjson(recv_socket_buffer);

    while (flag_get_supply == 1)
    {
        // in loop to get supply until SIGINT is ejecuted
        fprintf(stdout, "\n\tCTRL + C to back\n");
        send_message_to_server();
        recv_and_check_message();
    }

    return 0;
}

void end_client_conn()
{
    close(sockfd);
    clear_screen();
    // print message
    fprintf(stdout, "\n***Connection closed, terminating execution...***\n");
    sleep(1);
    exit(EXIT_SUCCESS);
}
void get_credentials()
{
    printf("Enter your username: ");
    fgets(username, MAX_USERNAME_LENGTH, stdin);
    username[strlen(username) - 1] = '\0';

    printf("Enter your password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strlen(password) - 1] = '\0';
}
void add_credentials()
{
    cjson_add_key_value_to_json_string(send_socket_buffer, K_HOSTNAME, username, OVERRIDE);
    cjson_add_key_value_to_json_string(send_socket_buffer, K_PASSWORD, password, OVERRIDE);
}

void get_options()
{
    int option = 0;
    int flag = 0;
    do
    {
        clear_screen();
        flag = 0;
        printf("Please enter an option (1 or 2):\n");
        fprintf(stdout, "1. %s\n", OPTION1_EQ);
        fprintf(stdout, "2. %s\n", OPTION2_EQ);
        fprintf(stdout, "3. %s\n", OPTION_END_EQ);
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            fprintf(stdout, "You have selected: %s.\n", OPTION1_EQ);
            flag_get_supply = 1;
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND, OPTION1, OVERRIDE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND_EQ, OPTION1_EQ, OVERRIDE);
            break;
        case 2:
            fprintf(stdout, "You have selected: %s.\n", OPTION2_EQ);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND, OPTION2, OVERRIDE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_COMMAND_EQ, OPTION2_EQ, OVERRIDE);
            get_supplies_options();
            break;
        case OPTION_END:
            fprintf(stdout, "You have selected: %s.\n", OPTION_END_EQ);
            end_client_conn();
            break;
        default:
            printf("Invalid option. Please enter a valid number.\n");
            flag = 1;
            option = 0;
            break;
        }
    } while (flag);
}

void get_supplies_options()
{
    int option = 0, flag = 0, value = 0;
    char key[MAX_K_V_SUP_LENGHT];
    do
    {
        clear_screen();
        flag = 0;
        printf("Please enter an option (1 or 2):\n");
        fprintf(stdout, "1. %s\n", SUP_OPT1_EQ);
        fprintf(stdout, "2. %s\n", SUP_OPT2_EQ);
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            fprintf(stdout, "You have selected: %s.\n", SUP_OPT1_EQ);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_SUP_COMMAND, SUP_OPT1, OVERRIDE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_SUP_COMMAND_EQ, SUP_OPT1_EQ, OVERRIDE);
            break;
        case 2:
            fprintf(stdout, "You have selected: %s.\n", SUP_OPT2_EQ);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_SUP_COMMAND, SUP_OPT2, OVERRIDE);
            cjson_add_key_value_to_json_string(send_socket_buffer, K_SUP_COMMAND_EQ, SUP_OPT2_EQ, OVERRIDE);
            break;
        default:
            printf("Invalid option. Please enter a valid number.\n");
            flag = 1;
            option = 0;
            break;
        }
    } while (flag);

    printf("Write the suply:\n");
    scanf("%s", key);
    cjson_add_key_value_to_json_string(send_socket_buffer, K_KEY, key, OVERRIDE);

    printf("Write the amount:\n");
    scanf("%d", &value);
    char value_str[10];
    sprintf(value_str, "%d", value);
    cjson_add_key_value_to_json_string(send_socket_buffer, K_VALUE, value_str, OVERRIDE);
}

void clear_screen()
{
    fprintf(stdout, "...\n");
    sleep(1);
    // ANSI secuence to clean the screen
    // printf("\033[2J\033[H");
    printf("\033[2J");
}

static void sign_handler(int signal)
{
    switch (signal)
    {
    case SIGINT:
        /* out of the loop*/
        printf("SIGINT called\n");
        flag_get_supply = 0;
        skip_option = 1;
        break;
    case SIGTSTP:
        printf("SIGTSTP called\n");
        // end connection
        end_client_conn();
        break;
    default:
        break;
    }
    return;
}

void set_signal_handlers()
{
    struct sigaction sa;
    sa.sa_handler = sign_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Couldn't set SIGINT handler");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTSTP, &sa, NULL) == -1)
    {
        perror("Couldn't set SIGTSTP handler");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("Couldn't set SIGTERM handler");
        exit(EXIT_FAILURE);
    }
}
