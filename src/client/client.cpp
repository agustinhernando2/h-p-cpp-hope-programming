#include <client.hpp>

int main(int argc, char const *argv[])
{
    // set_signal_handlers();
    std::unique_ptr<IConnection> con;

    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <host> IPV\n";
        std::cerr << "Example: " << argv[0] << " localhost is_ipv4\n";
        std::exit(EXIT_SUCCESS);
    }
    // get arg[2]
    int is_ip_version_4 = std::atoi(argv[2]);
    


    UserCredentials user;
    user.get_credentials();
    user.get_user_level();

    int user_level = user.get_user_level();
    int connection_type = (user_level) ? TCP : UDP;
    
    std::string address = (is_ip_version_4) ? "127.0.0.1" : "::1";
    std::string port = (is_ip_version_4) ? "8080" : "8081";

    std::cout << "Connecting to " << address << ":" << port << std::endl;

    con = std::make_unique<TCPv4Connection>(address, port, true);
    con->connect();

    int serverSocket = con->getSocket();
    if (serverSocket != -1) {
        std::cout << "Connected successfully.\n";
        std::string response1 = con->receiveFrom(serverSocket);
        std::cout << "Received: " << response1 << std::endl;
        con->sendto(user.get_username(),serverSocket);

        // std::string response2 = con->receive();
        // std::cout << "Received: " << response2 << std::endl;
        // con->send(user.get_password());
    } else {
        std::cerr << "Connection failed.\n";
        return 1;
    }

    // Crear una instancia de TCPv4Connection para el cliente
    TCPv4Connection client("127.0.0.1", "8080", true);

    return 0;
}

void UserCredentials::get_credentials() {
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    std::cout << "Enter your password: ";
    std::getline(std::cin, password);
}

int UserCredentials::get_user_level() {
    return 1;
}

std::string UserCredentials::get_username() const {
    return username;
}

std::string UserCredentials::get_password() const {
    return password;
}

/*int main(int argc, char *argv[])
{
    

    if (argc < 3)
    {
        fprintf(stderr, "Uso %s <host> IPV\n", argv[0]);
        fprintf(stderr, "Example %s localhost 4\n", argv[0]);
        fprintf(stderr, "Example %s localhost 6\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    con = gethostbyname(argv[1]);

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

    try_connect_con(connection_type, ip_version);

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
        send_message_to_con();
        recv_and_check_message();
    }
    return 0;
}*/