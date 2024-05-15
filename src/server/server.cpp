#include <cppSocketLib.hpp>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

int main() {

    // Dirección y puerto local
    std::string address = "127.0.0.1";
    std::string port = "8080";
    int cont = 0 ;
    // Creamos el servidor TCPv4
    auto con1 = createConnection(address, port, false, TCP);
    // Bind el servidor
    if (con1->bind()) {
        std::cout << "Servidor TCPv4 escuchando en " << address << ":" << port << std::endl;
        
        // Espera por conexiones
        while (true) {
            // Acepta una nueva conexión
            int clientfd = con1->connect();

            if (clientfd < 0) {
                std::cerr << "Error al aceptar la conexión." << std::endl;
                continue;
            }
            
            std::cout << "Conexión aceptada. Esperando mensajes del cliente..." << std::endl;
        
            while(true) {
              std::string message = con1->receiveFrom(clientfd);
              std::cout << "Received message: " << message << std::endl;
            }
        }
    } else {
        std::cerr << "Error al enlazar el servidor TCPv4." << std::endl;
        return 1;
    }

    return 0;
}
