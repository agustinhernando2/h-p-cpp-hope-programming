#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <fstream>
#include <semaphore>
#include <cstdint> 

#include <omp.h>
#include <nlohmann/json.hpp>
#include <httplib.h>

#include <rocksDbWrapper.hpp>
#include <supplies_data_handler.hpp>
#include <cannyEdgeFilter.hpp>
#include <cppSocketLib.hpp>
#include <alert_module.h>
#include <emergency_module.h>
#include <file_comunication_handler.hpp>

// Global semaphore
std::counting_semaphore<3> semaphore(1);

// Localhost IP
const char* localhost_ipv4 = "127.0.0.1";
const char* localhost_ipv6 = "::1";
const char* all_interfaces = "0.0.0.0";
// Localhost ports
const char* tcp4_port = "8080";
const char* tcp6_port = "8081";
const int http_port = 8888;
// Rocks db path
const std::string rockdb_path = "data/database.db";
// Path to save the images
const std::string img_path = "img_svr/";
// Queue message id
int msg_id = 0;

volatile sig_atomic_t signal_end_conn = 0;

// Constants for the user options
struct Option {
    int number;
    std::string command;
    std::string description;
};
// User options
const Option option1 = {1, "get_supplies", "Get supplies from server"};
const Option option2 = {2, "set_supplies", "Set supplies to server"};
const Option option3 = {3, "img_filtering", "Send image to process"};
const Option option4 = {4, "get_img_filtered", "Get image filtered from server"};
const Option option5 = {5, "end_conn", "End Connection"};

// Image names
const std::string img_names[5] = {"sobelDirection.png", "sobelMagnitude.png", "canny.png", "maxsupress.png", "gauss.png"};
/**
 * @brief Alert listener, when some alert is received, it will be printed and 
 * increased the alert counter
*/
void alert_listener();

/**
 * @brief Emergency listener, when some emergency is received, it will be printed and
 * saved in the emergency log file
*/
void run_emergency_listener(int fd);

/**
 * @brief Run the server and wait for clients
*/
int run_server(std::string address, std::string port, int protocol);

/**
 * @brief Handle the client connections
*/
void handle_client(IConnection* server, int clientFd);

/**
 * @brief Initialize the server http to receive GET and POST requests
*/
void start_http_server();

/**
 * @brief End the connection with the client
*/
void end_conn(int fd);

/**
 * @brief Get the command from the message
 * @param message: the command received from the client
 * @return the command number
*/
int get_command(std::string message);

void start_db();

void signal_handler(int signal);

int get_image(std::string message, int clientFd);

void send_image_file(IConnection* con, std::string img_name, int fd);

std::string get_image_selected(IConnection* con, int fd);

void send_images_names(IConnection* con, int clientFd);
