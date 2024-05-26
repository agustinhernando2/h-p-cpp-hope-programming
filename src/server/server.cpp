
#include <server.hpp>


using namespace std::chrono_literals;

int main() {
    start_db();
    std::vector<std::jthread> serverThreads;    
    // Start servers for IPv4
    std::cout << "Server IPv4 TCP listening: " << localhost_ipv4 << ":" << tcp4_port << std::endl;
    serverThreads.emplace_back([] { run_server(localhost_ipv4, tcp4_port, TCP); });

    // Start servers for IPv6
    std::cout << "Server IPv6 TCP listening: " << localhost_ipv6 << ":" << tcp6_port << std::endl;
    serverThreads.emplace_back([] { run_server(localhost_ipv6, tcp6_port, TCP); });

    // Start HTTP server
    serverThreads.emplace_back([] { start_http_server(); });

    // Wait for all threads to finish
    for (auto& thread : serverThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}

int run_server(std::string address, std::string port, int protocol){
    std::unique_ptr<IConnection> server = createConnection(address, port, true, protocol);
    if (server->bind()) {
        // Espera por conexiones
        while (true) {
            // Acepta una nueva conexión
            int clientFd = server->connect();

            if (clientFd < 0) {
                std::cerr << "Accept error." << std::endl;
                continue;
            }
            std::cout << "Connection accepted. Waiting for messages from the client..." << std::endl;

            // Maneja la conexión del cliente en un hilo separado
            std::jthread clientThread(handleClient, server.get(), clientFd); // Pasamos el puntero subyacente con get()
            clientThread.detach(); // Liberar el hilo para que pueda ejecutarse de manera independiente
        }
    } else {
        return 1;
    }

    return 0;
}

void handleClient(IConnection* server, int clientFd) {
    // Internal client to access the HTTP server
    httplib::Client icli(localhost_ipv4, http_port);

    std::string message;
    std::string status;
    try {
        while (true) {
            // Recibe un mensaje del cliente
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
                if(message.empty() || status == "404"){
                    message = "No supplies found";
                }
                server->sendto(message,clientFd);
                break;
            case 2:
                std::cout << "Setting supplies..." << std::endl;
                res = icli.Post("/setsupplies",message, "application/json");
                status = res->status;
                message = res->body;
                if(message.empty() || status == "404"){
                    message = "No supplies found";
                }
                server->sendto(message,clientFd);
                break;

            case 3:
                //CANNY EDGE FILTER
                std::cout << "Canny edge filter ..." << std::endl;
                std::this_thread::sleep_for(1s);
                break;
            case 4:
                end_conn();
                break;
            default:
                fprintf(stdout, "Command error\n");
                break;
            };

                
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl; 
    }
    
    close(clientFd);
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

void start_db() {
    std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>("data/database.db");

    try
    {
        // JSON parser
        nlohmann::json data = nlohmann::json::parse(json_data);

        // Insert
        db->put("alerts", data["alerts"].dump());
        db->put(SUPPLIES_KEY, data[SUPPLIES_KEY].dump());
        db->put("emergency", data["emergency"].dump());

        std::cout << "Database inicialized." << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

int get_command(std::string message){
    nlohmann::json j = nlohmann::json::parse(message);
    std::string command = j["command"];
    if(command == option1.command){
        return 1;
    }else if(command == option2.command){
        return 2;
    }else if(command == option3.command){
        return 3;
    }else if(command == option4.command){
        return 4;
    }else{
        return 0;
    }
}
void end_conn()
{
    std::cout << "Ending connection..." << std::endl;
    std::this_thread::sleep_for(2s);
    exit(EXIT_SUCCESS);
}
void start_http_server() {
    httplib::Server srv;

    srv.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello World!", "text/plain");
    });

    srv.Get("/supplies", [](const httplib::Request &, httplib::Response &res) {
        std::cout << "GET /supplies" << std::endl;

        std::string supplies = get_supplies();

        if (!supplies.empty()) {
            res.set_content(supplies, "application/json");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Get("/database", [](const httplib::Request &, httplib::Response &res) {
        std::cout << "GET /database" << std::endl;

        std::string result;
        std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>("data/database.db");
        nlohmann::json j;

        // Insert
        db->get("alerts", result);
        j["alerts"] = nlohmann::json::parse(result);

        db->get(SUPPLIES_KEY, result);
        j[SUPPLIES_KEY] = nlohmann::json::parse(result);
        
        db->get("emergency", result);
        j["emergency"] = nlohmann::json::parse(result);

        result = j.dump(4);

        if (!result.empty()) {
            res.set_content(result, "application/json");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Get("/alert", [](const httplib::Request &, httplib::Response &res) {

        std::cout << "GET /alert" << std::endl;
             
        // if (!content.empty()) {
        if (1) {
            res.set_content("content", "application/json");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    srv.Post("/setsupplies", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "POST /setsupplies" << std::endl;

        nlohmann::json j = nlohmann::json::parse(req.body);
        if(j.empty()){
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }else{
            set_supply(req.body);
            std::string supplie_added = "Supply added\n...\n" + j.dump(4);
            res.set_content(supplie_added, "application/json");
        }
    });

    // listen all interfaces
    srv.listen(localhost_ipv4, http_port);
}
