#include <server.hpp>
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
    std::cout << "Cliente UDP iniciado." << std::endl;
    // Crear una conexion UDP en el servidor
    std::shared_ptr<IConnection> con1 = std::make_shared<UDPConnection>("127.0.0.1", "7658", true, false);

    std::jthread thread1([&con1]() {
      con1->bind();
      while(true) {
        std::string message = con1->receive();
        std::cout << "Received message: " << message << std::endl;
      }
    });

    // Crear una conexión UDP en el cliente
    UDPConnection clientConnection("127.0.0.1", "7658", true, false); // IPv4

    // Conectar el socket
    clientConnection.connect();
    std::string message = "Hola desde el cliente";
    clientConnection.send(message);
    message = "Hola desde el cliente";
    clientConnection.send(message);
    message = "Hola desde el cliente";
    clientConnection.send(message);
    message = "Hola desde el cliente";
    clientConnection.send(message);
    message = "Hola desde el cliente";
    clientConnection.send(message);
    return 0;
}