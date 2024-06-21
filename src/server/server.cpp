
#include <server.hpp>

using namespace std::chrono_literals;

int main()
{
    std::signal(SIGINT, signal_handler);  // SIGINT (Ctrl+C)
    std::signal(SIGTSTP, signal_handler); // SIGTSTP (Ctrl+Z)
    std::signal(SIGTERM, signal_handler); // SIGTERM
    // Create the message queue
    msg_id = create_message_queue();

    std::vector<std::jthread> serverThreads;
    // Start servers for IPv4
    std::cout << "Server IPv4 TCP listening: " << localhost_ipv4 << ":" << tcp4_port << std::endl;
    serverThreads.emplace_back([] { run_server(localhost_ipv4, tcp4_port, TCP); });

    // Start servers for IPv6
    std::cout << "Server IPv6 TCP listening: " << localhost_ipv6 << ":" << tcp6_port << std::endl;
    serverThreads.emplace_back([] { run_server(localhost_ipv6, tcp6_port, TCP); });

    // Start HTTP server
    serverThreads.emplace_back([] { start_http_server(); });

    // Start Emergency Module
    serverThreads.emplace_back([] { run_emergency_module(); });
    // Start Alert Module
    serverThreads.emplace_back([] { run_alert_module(); });
    // Start Listener alert module
    serverThreads.emplace_back([] { alert_listener(); });

    // Wait for all threads to finish
    for (auto& thread : serverThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return 0;
}

int run_server(std::string address, std::string port, int protocol)
{
    // std::unique_ptr<IConnection> server = createConnection(address, port, true, protocol);
    server = createConnection(address, port, true, protocol);
    if (server->bind())
    {
        while (true)
        {
            // Accept connection
            clientFd = server->connect();

            if (clientFd < 0)
            {
                std::cerr << "Accept error." << std::endl;
                continue;
            }
            std::cout << "Connection accepted. Waiting for messages from the client..." << std::endl;

            // Manage the client in a new thread
            std::jthread clientThread(handle_client);
            clientThread.detach(); // free the thread resources
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

void handle_client()
{

    // Manage the client in a new thread
    std::jthread emergency_listener(run_emergency_listener);
    emergency_listener.detach(); // free the thread resources

    // Internal client to access the HTTP server
    httplib::Client icli(localhost_ipv4, http_port);

    std::string message;
    int status;
    try
    {
        while (true)
        {
            // Receive message from client
            message = server->receiveFrom(clientFd);
            std::cout << "PID: " << getpid() << std::endl;
            std::cout << "Message received: " << message << std::endl;
            std::this_thread::sleep_for(500ms);

            httplib::Result res;
            switch (get_command(message))
            {
            case 1:
                std::cout << "Getting supplies..." << std::endl;
                res = icli.Get("/supplies");
                status = res->status;
                message = res->body;
                if (message.empty() || status == 404)
                {
                    message = "No supplies found";
                }
                server->sendto(message, clientFd);
                break;
            case 2:
                std::cout << "Setting supplies..." << std::endl;
                res = icli.Post("/setsupplies", message, "application/json");
                status = res->status;
                message = res->body;
                if (message.empty() || status == 404)
                {
                    message = "No supplies found";
                }
                server->sendto(message, clientFd);
                break;

            case 3: {
                std::cout << "Canny edge filter ..." << std::endl;
                try
                {
                    nlohmann::json j = nlohmann::json::parse(message);
                    unsigned long compressedSize = j["compressedSize"].get<unsigned long>();
                    int num_threads = j["num_threads"].get<int>();           // number of threads
                    std::string img_name = j["img_name"].get<std::string>(); // image name

                    // Extract the name of the file
                    std::string compressedFile;
                    std::size_t dot_position = img_name.rfind('.');
                    if (dot_position != std::string::npos)
                    {
                        compressedFile = img_name.substr(0, dot_position);
                    }

                    omp_set_num_threads(num_threads);
                    std::cout << "Number of threads: " << num_threads << std::endl;

                    // Receive image
                    std::string file_name_comp = img_path + compressedFile + extension;
                    std::vector<char> file_data = receive_vector(clientFd, file_name_comp, compressedSize);

                    // Decompress image
                    std::string fileName = img_path + img_name;
                    decompressFile(file_data, fileName);

                    // call the cannyEdgeDetection function
                    EdgeDetection edgeDetection(40.0, 80.0, 1.0);
                    edgeDetection.cannyEdgeDetection(fileName);
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    end_conn();
                }
                break;
            }
            case 4: {
                try
                {
                    send_images_names();
                    std::string img_name = get_image_selected();
                    send_image_file(img_name);
                    break;
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    end_conn();
                }
            }
            case 5:
                end_conn();
                break;
            default:
                fprintf(stdout, "Command error\n");
                break;
            };
        }
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    close(clientFd);
}

void send_images_names()
{
    nlohmann::json j;
    // Add image as a key and img_names as an array value
    j["images"] = img_names;
    std::string message = j.dump();
    server->sendto(message, clientFd);
}

std::string get_image_selected()
{
    std::string message = server->receiveFrom(clientFd);
    nlohmann::json j = nlohmann::json::parse(message);
    std::string img_name = j["img_name"].get<std::string>();
    return img_name;
}

void send_image_file(std::string img_name)
{
    nlohmann::json j;
    j["img_name"] = img_name;

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
        server->sendto(jsonString, clientFd);
        // Send the compressed file
        send_vector(clientFd, file_data, compressedSize);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "Error sending data" << std::endl;
        j["command"] = "error";
        j["message"] = "Please try again, maybe the file does not exist or the name is incorrect";
        std::string message = j.dump();
        server->sendto(message, clientFd);
        std::cout << j["message"].get<std::string>() << std::endl;
        std::this_thread::sleep_for(1s);
        return;
    }
    std::string message = "File sent successfully";
    std::cout << message << std::endl;
    generate_log(message);
}

void alert_listener()
{
    mess_t send_buffer;

    while (true)
    {
        if (msgrcv(msg_id, &send_buffer, sizeof(send_buffer), 1, 0) == -1)
        {
            // print errno
            perror("msgrcv error");
            std::cerr << "1msg_id" << msg_id << std::endl;
            std::this_thread::sleep_for(1s);
            continue;
        }

        std::cout << "Alert received: " << send_buffer.message << std::endl;

        try
        {
            // Open db
            semaphore.acquire();
            std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);

            // Parse alert received
            nlohmann::json alert_received = nlohmann::json::parse(send_buffer.message);

            std::string message = send_buffer.message;
            generate_log(message);

            // Get alert location
            std::string location = alert_received["location"].get<std::string>();
            std::string alerts;

            // Get alerts from db
            db->get(K_ALERTS, alerts);

            // check if alerts is empty
            if (alerts.empty())
            {
                alerts = "{}";
            }

            nlohmann::json alerts_in_db = nlohmann::json::parse(alerts);

            alerts_in_db[location] = alerts_in_db[location].get<int>() + 1;

            // update
            db->put(K_ALERTS, alerts_in_db.dump());
        }
        catch (const std::exception& e)
        {
            std::string message = "Error processing alert: ";
            std::cerr << message << e.what() << std::endl;
            generate_log(message);
        }
        semaphore.release();
        std::string message = "Alert received";
        generate_log(message);
    }
}

void run_emergency_listener()
{
    mess_t send_buffer;
    while (true)
    {
        if (msgrcv(msg_id, &send_buffer, sizeof(send_buffer), 2, 0) == -1)
        {
            // print errno
            perror("msgrcv error");
            std::cerr << "2msg_id" << msg_id << std::endl;
            std::this_thread::sleep_for(1s);
            continue;
        }

        std::cout << "Emergency received: " << send_buffer.message << std::endl;

        try
        {
            // Open db
            semaphore.acquire();
            std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);

            // Parse alert received
            nlohmann::json emergency_received = nlohmann::json::parse(send_buffer.message);

            std::string message = send_buffer.message;

            // Get alert location
            std::string last_keepalived = emergency_received[LAST_KEEP_ALIVED].get<std::string>();
            std::string last_event = emergency_received[LAST_EVENT].get<std::string>();

            generate_log(last_keepalived);
            generate_log(last_event);
            // Get alerts from db
            std::string emergency;
            db->get(K_EMERGENCY, emergency);

            // check if alerts is empty
            if (emergency.empty())
            {
                emergency = "{}";
            }

            nlohmann::json emergency_in_db = nlohmann::json::parse(emergency);

            emergency_in_db[LAST_KEEP_ALIVED] = last_keepalived;
            emergency_in_db[LAST_EVENT] = last_event;

            // update
            db->put(K_EMERGENCY, emergency_in_db.dump());
            // End connection and exit
        }
        catch (const std::exception& e)
        {
            std::string message = "Error processing emergency: ";
            std::cerr << message << e.what() << std::endl;
            generate_log(message);
        }
        semaphore.release();
        std::string message = "Emergency received";
        generate_log(message);
        end_conn();
    }
}

int get_command(std::string message)
{

    nlohmann::json j = nlohmann::json::parse(message);
    std::string command = j["command"];
    if (command == option1.command)
    {
        return 1;
    }
    else if (command == option2.command)
    {
        return 2;
    }
    else if (command == option3.command)
    {
        return 3;
    }
    else if (command == option4.command)
    {
        return 4;
    }
    else if (command == option5.command)
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

void end_conn()
{
    std::string message = "Ending connection...";
    std::cout << message << std::endl;
    try
    {
        generate_log(message);

        // Send end connection message
        if (clientFd > 0)
        {
            std::cout << "Sending end connection message..." << std::endl;

            nlohmann::json j;
            j["message"] = "Connection ended";
            j["command"] = "end";
            message = j.dump();
            server->sendto(message, clientFd);
        }

        std::this_thread::sleep_for(2s);
        // Close message queue
        msgctl(msg_id, IPC_RMID, NULL);
        // Close connection using smart pointers
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    exit(EXIT_SUCCESS);
}
void start_http_server()
{
    httplib::Server srv;

    srv.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        std::cout << "GET /hi" << std::endl;
        res.set_content("Hello World!", "text/plain");
    });

    srv.Get("/supplies", [](const httplib::Request&, httplib::Response& res) {
        std::string message = "Getting supplies...";
        generate_log(message);
        std::cout << message << std::endl;

        std::string supplies = get_supplies();

        if (!supplies.empty())
        {
            res.set_content(supplies, "application/json");
        }
        else
        {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Get("/deletesupplies", [](const httplib::Request&, httplib::Response& res) {
        std::string message = "Deleting supplies...";
        generate_log(message);
        std::cout << message << std::endl;

        delete_supplies();
        res.set_content("Supply Deletion Successful", "application/json");
    });

    srv.Get("/initdb", [](const httplib::Request&, httplib::Response& res) {
        std::string message = "Initializing db...";
        generate_log(message);
        std::cout << message << std::endl;

        start_db();
        res.set_content("Successful initialization", "application/json");
    });

    srv.Get("/database", [](const httplib::Request&, httplib::Response& res) {
        std::string message = "Getting database...";
        std::cout << message << std::endl;
        generate_log(message);

        std::string result;
        std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);
        nlohmann::json j;

        // Insert
        db->get(K_ALERTS, result);
        j[K_ALERTS] = nlohmann::json::parse(result);

        db->get(SUPPLIES_KEY, result);
        j[SUPPLIES_KEY] = nlohmann::json::parse(result);

        db->get("emergency", result);
        j["emergency"] = nlohmann::json::parse(result);

        result = j.dump(4);

        if (!result.empty())
        {
            res.set_content(result, "application/json");
        }
        else
        {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Get("/alert", [](const httplib::Request&, httplib::Response& res) {
        std::string message = "Getting alerts...";
        std::cout << message << std::endl;
        generate_log(message);

        std::string alerts;
        std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);
        nlohmann::json j;

        // Get alerts
        db->get(K_ALERTS, alerts);
        j[K_ALERTS] = nlohmann::json::parse(alerts);
        // Format the json
        alerts = j[K_ALERTS].dump(4);

        if (1)
        {
            res.set_content(alerts, "application/json");
        }
        else
        {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Post("/setsupplies", [](const httplib::Request& req, httplib::Response& res) {
        std::string message = "Setting supplies...";
        std::cout << message << std::endl;
        generate_log(message);

        nlohmann::json j = nlohmann::json::parse(req.body);
        if (j.empty())
        {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
        else
        {
            semaphore.acquire();
            set_supply(req.body);
            semaphore.release();
            std::string supplie_added = "Supply added\n...\n" + j.dump(4);
            res.set_content(supplie_added, "application/json");
        }
    });

    // listen all interfaces
    srv.listen(localhost_ipv4, http_port);
}

// Datos JSON
const std::string json_data = R"(
{
    "alerts": {
        "NORTH": 74,
        "EAST": 70,
        "WEST": 70,
        "SOUTH": 65
    },
    "supplies": {
        "food": {
            "vegetables": 200,
            "fruits": 200,
            "grains": 200,
            "meat": 200
        },
        "medicine": {
            "antibiotics": 1,
            "analgesics": 100,
            "antipyretics": 100,
            "antihistamines": 100
        }
    },
    "emergency": {
        "last_keepalived": "Sun May 12 22:18:48",
        "last_event": "Sun May 12 22:18:48, Server failure. Emergency notification sent to all connected clients."
    }
}
)";

void start_db()
{
    std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);

    try
    {
        // JSON parser
        nlohmann::json data = nlohmann::json::parse(json_data);

        // Insert
        db->put(K_ALERTS, data[K_ALERTS].dump());
        db->put(SUPPLIES_KEY, data[SUPPLIES_KEY].dump());
        db->put("emergency", data["emergency"].dump());

        std::cout << "Database inicialized." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void signal_handler(int signal)
{
    std::cout << "Signal " << signal << " received" << std::endl;
    switch (signal)
    {
    case SIGINT:
        end_conn();
        break;
    default:
        break;
    }
}
