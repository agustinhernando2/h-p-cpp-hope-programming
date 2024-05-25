#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <cppSocketLib.hpp>

#define ADMIN 1
#define OTHER 0
#define TCP 1
#define UDP 2

// Server IP
const char* SERVER_IP = "127.0.0.1"; 
// Server port
const int SERVER_PORT = 8080; 

class UserCredentials {
private:
    std::string username;
    std::string password;

public:
    void get_credentials();
    int get_user_level();
    std::string get_username() const;
    std::string get_password() const;
};

void get_credentials();