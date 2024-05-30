#include <client.hpp>

using namespace std::chrono_literals;

int main(int argc, char const* argv[])
{
    std::signal(SIGINT, signal_handler);  // SIGINT (Ctrl+C)
    std::signal(SIGTSTP, signal_handler); // SIGTSTP (Ctrl+Z)
    std::signal(SIGTERM, signal_handler); // SIGTERM
    std::unique_ptr<IConnection> con;
    std::string response;
    int serverSocket = 0;
    int user_level = 0;
    int connection_type = 0;
    int is_ip_version_4 = 0;

    if (argc < 3)
    {
        std::cerr << "Uso: " << argv[0] << " <host> IPv4\n";
        std::cerr << "Example: " << argv[0] << " localhost 1\n";
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
    std::string port = (is_ip_version_4) ? tcp4_port : tcp6_port;

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
    int nproc = 0;
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

                        std::cout << "Enter the number of threads: " << std::endl;
                        std::cin >> nproc;

                        send_image_filtering_command(user, img_name, nproc, serverSocket, con);
                        // receive
                        break;
                    case 4: {
                        try
                        {
                            std::cout << "You have selected: " << option4.description << "." << std::endl;

                            // Ask for the image names to get
                            send_get_options_image_filtered_command(user, serverSocket, con);
                            response = con->receiveFrom(serverSocket);

                            // get image name from user
                            std::string img_name = get_img_name(response);

                            // send get image filtered selected
                            send_get_image_filtered_command(user, img_name, serverSocket, con);

                            // get compressed size
                            response = con->receiveFrom(serverSocket);
                            nlohmann::json j = nlohmann::json::parse(response);
                            if (j["command"] == "error")
                            {
                                std::cout << "Error: " << j["message"].get<std::string>() << std::endl;
                                std::this_thread::sleep_for(2s);
                                break;
                            }

                            unsigned long compressedSize = j["compressedSize"].get<unsigned long>();

                            std::string compressedFile;
                            std::size_t dot_position = img_name.rfind('.');
                            if (dot_position != std::string::npos)
                            {
                                compressedFile = img_name.substr(0, dot_position);
                            }
                            // Receive image
                            std::string file_name_comp = img_path + compressedFile + extension;
                            std::vector<char> file_data = receive_vector(serverSocket, file_name_comp, compressedSize);
                            // Decompress image
                            std::string fileName = img_path + img_name;
                            decompressFile(file_data, fileName);
                            break;
                        }
                        catch (const std::exception& e)
                        {
                            std::cerr << e.what() << '\n';
                            std::this_thread::sleep_for(1s);
                            end_client_conn();
                        }
                    }
                    case 5:
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

std::string get_img_name(std::string response)
{
    clear_screen();
    nlohmann::json j = nlohmann::json::parse(response);
    // get array of images names
    std::vector<std::string> img_names = j["images"].get<std::vector<std::string>>();
    // print the images names and option number
    int option = 0;
    while (true)
    {
        for (int i = 0; i < (int)img_names.size(); i++)
        {
            std::cout << i + 1 << ". " << img_names[i] << std::endl;
        }
        std::cout << "Enter the option number: " << std::endl;
        std::cin >> option;
        if (option < 1 || option > (int)img_names.size())
        {
            std::cout << "Invalid option. Please enter a valid number." << std::endl;
        }
        else
        {
            break;
        }
    }
    return img_names[option - 1];
}
void send_get_image_filtered_command(UserCredentials user, std::string img_name, int serverSocket,
                                     std::unique_ptr<IConnection>& con)
{
    std::cout << "Making get image filtered options ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = "continue";
    j["img_name"] = img_name;

    // Convert the JSON object to a string
    std::string jsonString = j.dump();
    std::cout << "Sending: " << jsonString << std::endl;

    // Send the string to the server
    con->sendto(jsonString, serverSocket);
}

void send_get_options_image_filtered_command(UserCredentials user, int serverSocket, std::unique_ptr<IConnection>& con)
{
    std::cout << "Making get image filtered options ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = option4.command;

    // Convert the JSON object to a string
    std::string jsonString = j.dump();
    std::cout << "Sending: " << jsonString << std::endl;

    // Send the string to the server
    con->sendto(jsonString, serverSocket);
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
    std::cout << "5. " << option5.description << std::endl;
    std::cin >> option;
    return option;
}
void send_image_filtering_command(UserCredentials user, std::string img_name, int nproc, int serverSocket,
                                  std::unique_ptr<IConnection>& con)
{
    std::cout << "Making image filtering command ..." << std::endl;

    nlohmann::json j;
    j["username"] = user.get_username();
    j["password"] = user.get_password();
    j["command"] = option3.command;
    j["img_name"] = img_name;
    j["num_threads"] = nproc;

    // File path
    const std::string inputFile = img_path + img_name;

    // Extract the name of the file
    std::string fileName;
    std::size_t dot_position = img_name.rfind('.');
    if (dot_position != std::string::npos)
    { // npos is returned when the string is not found
        fileName = img_name.substr(0, dot_position);
    }
    else
    {
        std::cout << "Invalid file name" << std::endl;
        return;
    }
    const std::string compressedFile = img_path + fileName + extension;

    int inputSize = 0;
    unsigned long compressedSize = 0;
    try
    {
        std::vector<char> file_data = compressFile(inputFile, compressedFile, &inputSize, &compressedSize);
        j["compressedSize"] = compressedSize;
        // Convert the JSON object to a string
        std::string jsonString = j.dump();
        std::cout << "Sending: " << jsonString << std::endl;
        con->sendto(jsonString, serverSocket);
        // Send the compressed file
        send_vector(serverSocket, file_data, compressedSize);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "Error sending data" << std::endl;
        std::cout << "Please try again, maybe the file does not exist or the name is incorrect" << std::endl;
        std::this_thread::sleep_for(1s);
        return;
    }
    printf("Data sent successfully!\n");
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
    // compare username and password
    if (username == ADMIN_USER && password == ADMIN_PASS)
    {
        return 1;
    }
    return 0;
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

void signal_handler(int signal)
{
    std::cout << "Signal " << signal << " received" << std::endl;
    switch (signal)
    {
    case SIGTSTP:
        end_client_conn();
        break;
    default:
        break;
    }
    clear_screen();
    flag_opt1 = 0;
}