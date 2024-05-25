
#include <server.hpp>


int main() {

    std::vector<std::jthread> serverThreads;    
    // HTTP
    
    // Iniciar el servidor IPv4 en el puerto 8080   
    serverThreads.emplace_back([] { run_server(false, "8080"); });

    // Iniciar el servidor IPv6 en el puerto 8081
    serverThreads.emplace_back([] { run_server(true, "8081"); });
    serverThreads.emplace_back([] { start_http_server(); });

    // Esperar indefinidamente (o hasta que se interrumpa el programa)
    for (auto& thread : serverThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}

int run_server(bool is_ipv6, std::string port){
    std::unique_ptr<IConnection> server;
    std::string address;
    std::string type = is_ipv6 ? IPV6_ : IPV4_;
    //auto server ;
    if(is_ipv6){
        server = createConnection("::1", port, true, TCP);
    }else{
        server = createConnection("127.0.0.1", port, true, TCP);
    }
    if (server->bind()) {
        std::cout << "Server " << type << " listening: " << address << ":" << port << std::endl;
        
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
        std::cerr << "Server binding error " << type << ", " << address << ":" << port << std::endl;
        return 1;
    }

    return 0;
}

bool authentication(std::string username, std::string password) {
    // Verifica las credenciales del username
    std::cout << username << " " << password << std::endl;
    // Comparación de strings usando el operador ==
    if(check_json_users(username,password,"data/users.json")==true){
        return true;
    }else{
        return false;
    }
}
bool check_json_users(const std::string& username, const std::string& password, const std::string& path){
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        std::cerr << "No se puede abrir el archivo " << path << std::endl;
        return false;
    }

    nlohmann::json j;
    archivo >> j;

    for (const auto& item : j["usernames"]) {
        if (item["username"] == username && item["contraseña"] == password) {
            return true;
        }
    }

    return false;
}

void handleClient(IConnection* server, int clientFd) {
    // Maneja la conexión con el cliente
    while (true) {
        // Recibe datos del cliente
        try {

            //autenticacion
            server->sendto("¡Hola cliente! Ingrese su username ",clientFd);
            std::string username = server->receiveFrom(clientFd);
            std::cout << "Username: " << username << std::endl;
            // server->send("¡Hola cliente! Ingrese password ");
            // std::string password = server->receive();
            sleep(1000);
            // if (authentication(username, password) == false) {
            //     server->sendto("username o contraseña incorrectos", clientFd);
            //     close(clientFd);
            //     break;
            // }else{
            //     handler_principal(server, clientFd, username);
            // }
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break; 
        }
    }
    
    close(clientFd);
}

void handler_principal(IConnection* server, int clientFd, std::string user){
    while(true){
        try{
            server->sendto("Menu:\n1. Recibir resumen.\n2. Ingresar datos.\n3. Escanear imagen.\n4. Salir.\n", clientFd);
            std::string rta = server->receiveFrom(clientFd);
            switch (std::stoi(rta)) {
                case 1:
                    // Code for receiving summary
                    //server->sendto("Sim  summary\n", clientFd);
                    server->sendto(get_supplies(),clientFd);
                    break;
                case 2:
                    // Code for entering data
                    //server->sendto("Sim supply\n", clientFd);
                    supplies_data(server, clientFd, user);
                    break;
                case 3:
                    // Code for scanning image
                    server->sendto("Sim image\n", clientFd);
                    break;
                case 4:
                    // Code for exiting
                    server->sendto("Sim exit\n", clientFd);
                    break;
                default:
                    // Code for handling invalid input
                    server->sendto("Sim invalid", clientFd);
                    break;
            }
        }catch (const std::runtime_error& e) {
            std::cerr << "Error al recibir datos del cliente: " << e.what() << std::endl;
            break; // Salir del bucle interno si ocurre un error al recibir datos
        }
    }
    
}

std::string read_file(const std::string &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void start_http_server() {
    httplib::Server server;

    server.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello World!", "text/plain");
    });

    server.Get("/supplies", [](const httplib::Request &, httplib::Response &res) {
        std::string file_path = "data/state_summary.json";
        std::string content = read_file(file_path);
        if (!content.empty()) {
            res.set_content(content, "application/json");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });

    std::cout << "Servidor HTTP escuchando en el puerto 8082" << std::endl;
    server.listen("0.0.0.0", 8082);
}

std::string get_supplies(){
    httplib::Client cli("http://localhost:8082");

    auto res = cli.Get("/supplies");
    if (res) {
        //std::cout << "Status code: " << res->status << std::endl;
        //std::cout << "Response body: " << res->body << std::endl;
        return res->body;
    } else {
        std::cout << "Error: " << res.error() << std::endl;
        return "error";
    }
}

void supplies_data(IConnection* server, int clientFd, std::string user){
    nlohmann::json j;
    std::string key;
    std::string value;
    j["hostname"] = user;
    while(true){
        server->sendto("Ingrese la clave del articulo o 'fin' si desea salir: ", clientFd);
        std::string key = server->receiveFrom(clientFd);
        if(key == "fin"){
            break;
        }
        server->sendto("Ingrese el valor del articulo: ", clientFd);
        std::string value = server->receiveFrom(clientFd);
        j[key] = std::atoi(value.c_str());
    }

    std::string jsonString = j.dump();
    std::cout<< "JSON FORMADO: "<<jsonString<<std::endl;
}