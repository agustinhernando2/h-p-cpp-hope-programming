#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>

#include <nlohmann/json.hpp>
#include <httplib.h>
#include <unistd.h>

#include <rocksDbWrapper.hpp>
#include <supplies_data_handler.hpp>
#include <cannyEdgeFilter.hpp>
#include <cppSocketLib.hpp>
#include <alert_module.h>

// Localhost IP
const char* localhost_ipv4 = "127.0.0.1";
const char* localhost_ipv6 = "::1";
const char* all_interfaces = "0.0.0.0";
// Localhost ports
const char* tcp4_port = "8080";
const char* tcp6_port = "8081";
const int http_port = 8888;

int msg_id = 0;

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

void alert_listener();
int run_server(std::string address, std::string port, int protocol);
void handle_client(IConnection* server, int clientFd);
void start_http_server();
void end_conn();

int get_command(std::string message);

void start_db();
