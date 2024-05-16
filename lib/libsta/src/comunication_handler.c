#include <comunication_handler.h>

int connect_server_ipv4(int *sockfd, int type)
{
    struct sockaddr_in serv_addr;
    uint16_t port = 0;

    if ((*sockfd = socket(AF_INET, type, 0)) < 0)
    {
        error_handler("Error socket", __FILE__, __LINE__);
        return 1;
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    if (type == 1)
    {
        port = PORT_IPV4_TCP;
    }
    else
    {
        port = PORT_IPV4_UDP;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons((uint16_t)port);

    if (bind(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error_handler("Error bind", __FILE__, __LINE__);
        return 1;
    }

    // Print the protocol type used
    if (type == 1)
    {
        printf("** SERVIDOR IPv4 TCP - Proceso: %d - socket disponible: %d **\n", getpid(), ntohs(serv_addr.sin_port));
        if ((listen(*sockfd, N_CONN)) == -1)
        {
            error_handler("Error listen", __FILE__, __LINE__);
            return 1;
        }
    }
    else
    {
        printf("** SERVIDOR IPv4 UDP - Proceso: %d - socket disponible: %d **\n", getpid(), ntohs(serv_addr.sin_port));
    }

    return 0;
}

int connect_server_ipv6(int *sockfd, int type)
{
    struct sockaddr_in6 serv_addr6;
    uint16_t port = 0;

    if ((*sockfd = socket(AF_INET6, type, 0)) < 0)
    {
        error_handler("Error socket", __FILE__, __LINE__);
        return 1;
    }

    memset((char *)&serv_addr6, 0, sizeof(serv_addr6));
    if (type == 1)
    {
        port = PORT_IPV6_TCP;
    }
    else
    {
        port = PORT_IPV6_UDP;
    }
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_addr = in6addr_any;
    serv_addr6.sin6_port = htons((uint16_t)port);

    if (bind(*sockfd, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6)) < 0)
    {
        error_handler("Error bind", __FILE__, __LINE__);
        return 1;
    }

    // Print the protocol type used
    if (type == 1)
    {
        printf("** SERVIDOR IPv6 TCP - Proceso: %d - socket disponible: %d **\n", getpid(),
               ntohs(serv_addr6.sin6_port));
        if (listen(*sockfd, N_CONN) == -1)
        {
            error_handler("Error listen", __FILE__, __LINE__);
            return 1;
        }
    }
    else
    {
        printf("** SERVIDOR IPv6 UDP - Proceso: %d - socket disponible: %d **\n", getpid(),
               ntohs(serv_addr6.sin6_port));
    }

    return 0;
}

int connect_client_ipv4(int *sockfd, const char *ip_address, int type)
{
    struct sockaddr_storage serv_addr;
    socklen_t addr_size = 0;
    uint16_t port = 0;

    *sockfd = socket(AF_INET, type, 0);
    if (*sockfd < 0)
    {
        perror("Fallo al crear el socket");
        return -1;
    }

    if (type == 1)
    {
        port = PORT_IPV4_TCP;
    }
    else
    {
        port = PORT_IPV4_UDP;
    }
    struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)&serv_addr;
    ipv4_addr->sin_family = AF_INET;
    ipv4_addr->sin_port = htons(port);

    // Convertir la dirección IPv4
    if (inet_pton(AF_INET, ip_address, &(ipv4_addr->sin_addr)) <= 0)
    {
        fprintf(stderr, "Dirección IP inválida: %s\n", ip_address);
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in);

    if (connect(*sockfd, (struct sockaddr *)&serv_addr, addr_size) < 0)
    {
        perror("Fallo al conectar");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int connect_client_ipv6(int *sockfd, const char *ip_address, int type)
{
    struct sockaddr_storage serv_addr;
    socklen_t addr_size = 0;
    uint16_t port = 0;

    *sockfd = socket(AF_INET6, type, 0);
    if (*sockfd < 0)
    {
        perror("Fallo al crear el socket");
        return -1;
    }

    if (type == 1)
    {
        port = PORT_IPV6_TCP;
    }
    else
    {
        port = PORT_IPV6_UDP;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)&serv_addr;
    ipv6_addr->sin6_family = AF_INET6;
    ipv6_addr->sin6_port = htons(port);

    // Convertir la dirección IPv6
    if (inet_pton(AF_INET6, ip_address, &(ipv6_addr->sin6_addr)) <= 0)
    {
        perror("Dirección IP inválida");
        exit(EXIT_FAILURE);
    }
    addr_size = sizeof(struct sockaddr_in6);

    if (connect(*sockfd, (struct sockaddr *)&serv_addr, addr_size) < 0)
    {
        perror("Fallo al conectar");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int send_message(char *json_buffer, size_t b_size, int sockfd)
{
    size_t slice = 0;
    char socket_buffer[BUFFER_SIZE];
    memset(socket_buffer, 0, BUFFER_SIZE);

    if (b_size <= BUFFER_SIZE)
    {
        strncpy(socket_buffer, json_buffer, b_size);
        return (send_message_to_socket(socket_buffer, b_size, sockfd));
    }
    while (TRUE)
    {
        if ((slice + BUFFER_SIZE) > b_size)
        {
            break;
        }
        strncpy(socket_buffer, json_buffer + slice, BUFFER_SIZE);
        if (send_message_to_socket(socket_buffer, BUFFER_SIZE, sockfd))
        {
            fprintf(stderr, "%s:%d: Error send_message_to_socket.\n", __FILE__, __LINE__);
            return 1;
        }
        slice += BUFFER_SIZE;
    }
    strncpy(socket_buffer, json_buffer + slice, BUFFER_SIZE);
    return send_message_to_socket(socket_buffer, strlen(json_buffer), sockfd);
}

int send_message_to_socket(char *json_buffer, size_t b_size, int sockfd)
{
    if ((b_size == 0) || (strlen(json_buffer) == 0))
    {
        return 0;
    }
    if (send(sockfd, json_buffer, b_size, 0) == -1)
    {
        fprintf(stderr, "%s:%d: Error sending the json_buffer content.\n", __FILE__, __LINE__);
        return 1;
    }
    return 0;
}

int recv_tcp_message(int sockfd, char *socket_buffer)
{
    memset(socket_buffer, 0, BUFFER_SIZE);
    ssize_t n;
    n = recv(sockfd, socket_buffer, BUFFER_SIZE, 0);
    if ((n == -1) || (strlen(socket_buffer) == 0))
    {
        fprintf(stderr, "%s:%d: Error reciving the data.\n", __FILE__, __LINE__);
        return 1;
    }
    return 0;
}

int recv_udp_message(int sockfd, char *socket_buffer, struct sockaddr_storage *cli_addr)
{
    memset(socket_buffer, 0, BUFFER_SIZE);
    socklen_t clilen = sizeof(*cli_addr);
    ssize_t n;
    n = recvfrom(sockfd, socket_buffer, BUFFER_SIZE, 0, (struct sockaddr *)cli_addr, &clilen);
    if ((n == -1) || (strlen(socket_buffer) == 0))
    {
        fprintf(stderr, "%s:%d: Error reciving the data.\n", __FILE__, __LINE__);
        return 1;
    }
    return 0;
}

int send_udp_message(int newsockfd, char *send_socket_buffer, struct sockaddr_storage *cli_addr)
{
    if (sendto(newsockfd, send_socket_buffer, strlen(send_socket_buffer), 0, (struct sockaddr *)cli_addr,
               sizeof(*cli_addr)) == -1)
    {
        fprintf(stderr, "%s:%d: Error sending the json_buffer content.\n", __FILE__, __LINE__);
        return 1;
    }
    return 0;
}
