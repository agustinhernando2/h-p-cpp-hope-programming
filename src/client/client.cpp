#include <client.hpp>

int main(int argc, char const* argv[])
{
    std::signal(SIGINT, signal_handler);   // SIGINT (Ctrl+C)
    std::signal(SIGTSTP, signal_handler);  // SIGTSTP (Ctrl+Z)
    std::signal(SIGTERM, signal_handler);  // SIGTERM

    std::unique_ptr<IConnection> con;
    std::string response;
    int serverSocket = 0;
    int user_level = 0;
    int connection_type = 0;
    int is_ip_version_4 = 0;

    if (argc < 3)
    {
        std::cerr << "Uso: " << argv[0] << " <host> IPV\n";
        std::cerr << "Example: " << argv[0] << " localhost is_ipv4\n";
        std::exit(EXIT_SUCCESS);
    }

    UserCredentials user;
    user.get_credentials();
    user.get_user_level();

    user_level = user.get_user_level();
    connection_type = (user_level) ? TCP : UDP;

    // get ip version4 true / false
    is_ip_version_4 = std::atoi(argv[2]);
    std::string address = (is_ip_version_4) ? localhost_ipv4 : localhost_ipv6;
    std::string port = (is_ip_version_4) ? 
        (connection_type == TCP) ? tcp4_port : udp4_port : 
        (connection_type == TCP) ? tcp6_port : udp6_port;

    std::cout << "Connecting to " << address << ":" << port << std::endl;

    con = std::make_unique<TCPv4Connection>(address, port, true);
    con->connect();

    serverSocket = con->getSocket();
    if (serverSocket == -1)
    {
        std::cerr << "Connection failed.\n";
        return 1;
    }
    std::cout << "Connected successfully.\n";

    std::string sup_type;
    std::string sup_name;
    std::string sup_amount;

    std::string img_name;
    try
    {
        while (true)
        {
            if (connection_type == UDP || flag_opt1 == 1)
            {
                // Get the supplies in loop
                send_get_supplies_command(user, serverSocket, con);
                response = con->receiveFrom(serverSocket);
                std::cout << "Received: " << response << std::endl;
            }
            else
            {
                bool flag = false;
                do
                {
                    switch (get_options())
                    {
                    case 1:
                        std::cout << "You have selected: " << option1.description << "." << std::endl;
                        // send get supplies command
                        send_get_supplies_command(user, serverSocket, con);
                        // receive response and print
                        response = con->receiveFrom(serverSocket);
                        std::cout << "Received: " << response << std::endl;
                        // To get supplies in loop
                        flag_opt1 = 1;
                        break;
                    case 2:
                        std::cout << "You have selected: " << option2.description << "." << std::endl;

                        std::cout << "Enter the supply type: " << std::endl;
                        std::cin >> sup_type;
                        std::cout << "Enter the supply name: " << std::endl;
                        std::cin >> sup_name;
                        std::cout << "Enter the supply amount: " << std::endl;
                        std::cin >> sup_amount;

                        // to lower case
                        boost::algorithm::to_lower(sup_type);
                        boost::algorithm::to_lower(sup_name);
                        boost::algorithm::to_lower(sup_amount);
                        // send set supplies command
                        send_set_supplies_command(user, sup_type, sup_name, sup_amount, serverSocket, con);
                        // receive response and print
                        response = con->receiveFrom(serverSocket);
                        std::cout << "Received: " << response << std::endl;
                        break;
                    case 3:
                        std::cout << "You have selected: " << option3.description << "." << std::endl;
                        
                        std::cout << "Enter the image name: " << std::endl;
                        std::cin >> img_name;

                        // to lower case
                        boost::algorithm::to_lower(img_name);

                        send_image_filtering_command(user, img_name, serverSocket, con);
                        // receive 
                        break;
                    case 4:
                        std::cout << "You have selected: " << option4.description << "." << std::endl;
                        end_client_conn();
                        break;
                    default:
                        std::cout << "Invalid option. Please enter a valid number." << std::endl;
                        flag = true;
                        break;
                    }
                } while (flag);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}



void end_client_conn()
{
    std::cout << "ending client connection ..." << std::endl;
    clear_screen();
    exit(EXIT_SUCCESS);
}

int get_options()
{
    int option = 0;
    clear_screen();
    std::cout << "Please enter an option (1, 2, 3, or 4):" << std::endl;
    std::cout << "1. " << option1.description << std::endl;
    std::cout << "2. " << option2.description << std::endl;
    std::cout << "3. " << option3.description << std::endl;
    std::cout << "4. " << option4.description << std::endl;
    std::cin >> option;
    return option;
}
void send_image_filtering_command(UserCredentials user, std::string img_name, int serverSocket, std::unique_ptr<IConnection>& con)
{
    std::cout << "Making image filtering command ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = option3.command;
    j["image_name"] = img_name;

    // Convert the JSON object to a string
    std::string jsonString = j.dump();
    std::cout << "Sending: " << jsonString << std::endl;

    // Send the string to the server
    con->sendto(jsonString, serverSocket);
    //send_compressed_image(jsonString, serverSocket, con);
    clear_screen();
}

void send_set_supplies_command(UserCredentials user, std::string sup_type, std::string sup_name, std::string sup_amount,
                               int serverSocket, std::unique_ptr<IConnection>& con)
{
    std::cout << "Making set supplies command ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = option2.command;
    j["supply_type"] = sup_type;
    j["supply_name"] = sup_name;
    j["supply_amount"] = sup_amount;

    // Convert the JSON object to a string
    std::string jsonString = j.dump();
    std::cout << "Sending: " << jsonString << std::endl;

    // Send the string to the server
    con->sendto(jsonString, serverSocket);
    clear_screen();
}

void send_get_supplies_command(UserCredentials user, int serverSocket, std::unique_ptr<IConnection>& con)
{
    std::cout << "Making get supplies command ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = option1.command;

    // Convert the JSON object to a string
    std::string jsonString = j.dump();
    std::cout << "Sending: " << jsonString << std::endl;

    // Send the string to the server
    con->sendto(jsonString, serverSocket);
    clear_screen();
}

void UserCredentials::get_credentials()
{
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    std::cout << "Enter your password: ";
    std::getline(std::cin, password);
}

int UserCredentials::get_user_level()
{
    return 1;
}

std::string UserCredentials::get_username() const
{
    return username;
}

std::string UserCredentials::get_password() const
{
    return password;
}

void clear_screen()
{
    std::cout << "...\n";
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
    std::cout << "\033[2J\033[H";
}

void signal_handler(int signal) {
    std::cout << "Signal " << signal << " received" << std::endl;
    switch (signal)
    {
    case SIGTSTP:
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
    clear_screen();
    flag_opt1 = 0;
}