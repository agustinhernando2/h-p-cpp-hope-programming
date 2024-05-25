/*preventing multiple inclusions of the same header file*/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* Unix libraries */
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

/* Libraries */
// #include <emergency_handler.h>
// #include <alert_module.h>
// #include <comunication_handler.h>
// #include <supplies_data_handler.h>
// #include <lib_handler.h>
// #include <cjson_handler.h>

/* An integral type that can be modified atomically, without the
   possibility of a signal arriving in the middle of the operation.  */
 volatile sig_atomic_t flag_handler = 0;

int pipe_fd[2];

int setup();

sem_t mutex_state; 

// /**
//  * @brief Runs the server using IPv4.
//  *
//  * @param[in] type type udp/tcp.
//  */
// void run_server_ipv4(int type);

// /**
//  * @brief Runs the server using IPv6.
//  *
//  * @param[in] type type udp/tcp.
//  */
// void run_server_ipv6(int type);

// /**
//  * @brief Run the server.
//  *
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void run_admin_server(int newsockfd);

// /**
//  * @brief Run the server.
//  *
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void run_normal_user_server(int newsockfd, struct sockaddr_storage* cli_addr);

// void end_conn(int newsockfd);

// void send_deneid_udp_message(int newsockfd, struct sockaddr_storage* cli_addr);

// void send_supply_udp(int newsockfd, struct sockaddr_storage* cli_addr);

// /**
//  * @brief Modify and send the supply status to the client.
//  * only admin can do run this function.
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void set_and_send_suply_status(int newsockfd);

// /**
//  * @brief Sends a supply status message to the client.
//  *
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void send_supply_message(int newsockfd);

// /**
//  * @brief Send an end connection.
//  *
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void send_end_conn_message(int newsockfd);

// /**
//  * @brief Send a denied message to the client..
//  *
//  * @param[in] newsockfd The socket file descriptor for the connection.
//  */
// void send_deneid_message(int newsockfd);

// /**
//  * @brief Gets the command from the user.
//  *
//  * @return 0 on error command, positive number on success.
//  */
// int get_command();

// /**
//  * @brief check credentials
//  *
//  * @return 0 on success, 1 deneid.
//  */
// int check_credentials();

// /**
//  * @brief Signal handler function.
//  *
//  * @param[in] signal The signal received.
//  */
// static void sign_handler(int signal);

// /**
//  * @brief Sets signal handlers for interrupt signals.
//  */
// void set_signal_handlers();


// /**
//  * @brief Set the supply.
//  *
//  * @return 0 on success, -1 on error.
//  */
// int set_supply();

// void alert_and_emergency_listener(int msgid);

// /**
//  * @brief Create the message queue.
//  * @return msgid.
// */
// int create_message_queue();

// void hello_world_from_c();
#ifdef __cplusplus
}
#endif