#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <csignal>
#include <cppSocketLib.hpp>
#include <nlohmann/json.hpp>

// Constants for the user options
struct Option {
    int number;
    std::string command;
    std::string description;
};

const Option option1 = {1, "get_supplies","Get supplies from server"};
const Option option2 = {2, "set_supplies", "Set supplies to server"};
const Option option3 = {3, "img_filtering", "Send image to process"};
const Option option4 = {4, "end_conn", "End Connection"};

// Constants for user
const char* ADMIN_USER = "admin";
const char* ADMIN_PASS = "admin";
const int ADMIN_LEVEL = 1;
const int NORMAL_LEVEL = 0;

// Localhost IP
const char* localhost_ipv4 = "127.0.0.1";
const char* localhost_ipv6 = "::1";
// Localhost ports
const char* tcp4_port = "8080";
const char* udp4_port = "8090";
const char* tcp6_port = "8081";
const char* udp6_port = "8091";

// Connection types
// const int TCP = 1;
// const int UDP = 2;

// flag to option 1 in loop
int flag_opt1 = 0;

class UserCredentials {
private:
    std::string username;
    std::string password;

public:
    void get_credentials();
    int get_user_level();
    std::string get_username() const; // const member function, it does not modify the object
    std::string get_password() const; // const member function, it does not modify the object

};


void clear_screen();
int get_options();
void end_client_conn();
void signal_handler(int signal);

void send_get_supplies_command(UserCredentials user, int serverSocket, std::unique_ptr<IConnection>& con);
void send_image_filtering_command(UserCredentials user, std::string img_name, int serverSocket, std::unique_ptr<IConnection>& con);
void send_set_supplies_command(UserCredentials user, std::string sup_type, std::string sup_name, std::string sup_amount,
                               int serverSocket, std::unique_ptr<IConnection>& con);