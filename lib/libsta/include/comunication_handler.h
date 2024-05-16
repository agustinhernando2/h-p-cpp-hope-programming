#pragma once

#include <arpa/inet.h>
#include <lib_handler.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT_IPV4_UDP 2222
#define PORT_IPV4_TCP 3333
#define PORT_IPV6_UDP 4444
#define PORT_IPV6_TCP 5555
#define FILE_UNIX_UDP "unix_udp"
#define FILE_UNIX_TCP "unix_tcp"
#define N_CONN 5
#define IPV4 4
#define IPV6 6
#define IP_ADDR_V4 "127.0.0.1"
#define IP_ADDR_V6 "::1"

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_K_V_SUP_LENGHT 50
#define ADMIN "u"

#define SUP_OPT1 "1"
#define SUP_OPT1_EQ "food"
#define SUP_OPT2 "2"
#define SUP_OPT2_EQ "medicine"
#define OPTION_END 3
#define OPTION_END_EQ "End client connection"
#define CLOSE_CONNECTION 0
#define OPTION1_ 1
#define OPTION1 "1"
#define OPTION1_EQ "Get server status"
#define OPTION2_ 2
#define OPTION2 "2"
#define OPTION2_EQ "Modify supplies"
#define K_COMMAND "command"
#define K_COMMAND_EQ "command_equivalent"
#define K_SUP_COMMAND "suply_type"
#define K_SUP_COMMAND_EQ "suply_type_equivalent"

#define K_ACC_DENEID "deneid_command"
#define K_HOSTNAME "hostname"
#define K_PASSWORD "password"
#define K_END "end_connection"
#define K_VALUE "value"
#define K_KEY "key"

#define M_ACC_DENEID "You should be connected to the server using TCP and be an admin to modify the supplies."
#define C_SIZE 50
#define BUFFER_SIZE 1024
#define MESSAGE "message"
#define END "end"
#define TRUE 1
#define TRUE_ "1"
#define FALSE 0
#define FALSE_ "0"
#define DELIMITER '\n'

/**
 * @brief Connects to the server using IPv4.
 *
 * @param[out] sockfd Pointer to the socket file descriptor.
 * @param[in] type type udp/tcp.
 * @return 0 on success, -1 on error.
 */
int connect_server_ipv4(int *sockfd, int type);

/**
 * @brief Connects to the server using IPv6.
 *
 * @param[out] sockfd Pointer to the socket file descriptor.
 * @param[in] argv Command-line arguments.
 * @param[in] type type udp/tcp.
 * @return 0 on success, -1 on error.
 */
int connect_server_ipv6(int *sockfd, int type);

/**
 * @brief Connects to the server using IPv4.
 *
 * @param[out] sockfd Pointer to the socket file descriptor.
 * @param[in] ip_address address.
 * @param[in] type type udp/tcp.
 * @return 0 on success, -1 on error.
 */
int connect_client_ipv4(int *sockfd, const char *ip_address, int type);

/**
 * @brief Connects to the server using IPv6.
 * @param[out] sockfd Pointer to the socket file descriptor.
 * @param[in] ip_address address.
 * @param[in] type type udp/tcp.
 */
int connect_client_ipv6(int *sockfd, const char *ip_address, int type);
/**
 * @brief - send a message
 * If the message is too long so It'll be sending by parts
 * @param json_buffer: JSON character buffer.
 * @param sockfd: socket file descriptor.
 * @return 0 success, 1 if an error occurs.
 */
int send_message(char *json_buffer, size_t b_size, int sockfd);

/**
 * @brief - send a message through sockets
 *
 * @param json_buffer: JSON character buffer.
 * @param sockfd: socket file descriptor.
 * @return 0 success, 1 if an error occurs.
 */
int send_message_to_socket(char *json_buffer, size_t b_size, int sockfd);

/**
 * @brief - send a message through sockets
 *
 * @param json_buffer: JSON character buffer.
 * @param sockfd: socket file descriptor.
 * @return 0 success, 1 if an error occurs.
 */
int recv_tcp_message(int sockfd, char *json_buffer);

/**
 * @brief - send a message through sockets using UDP
 */
int recv_udp_message(int sockfd, char *socket_buffer, struct sockaddr_storage *cli_addr);

/**
 * @brief - send a message through sockets using UDP
 */
int send_udp_message(int newsockfd, char *send_socket_buffer, struct sockaddr_storage *cli_addr);
