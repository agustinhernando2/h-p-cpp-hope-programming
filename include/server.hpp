#pragma once
#include <csignal>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <semaphore>
#include <string>
#include <thread>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <omp.h>

#include <alert_module.h>
#include <cannyEdgeFilter.hpp>
#include <cppSocketLib.hpp>
#include <emergency_module.h>
#include <file_comunication_handler.hpp>
#include <rocksDbWrapper.hpp>
#include <supplies_data_handler.hpp>

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
const std::string database = "data/database.db";

// Path to save the images
const std::string img_path = "img_svr/";
// Queue message id
int msg_id = 0;

volatile sig_atomic_t signal_end_conn = 0;

// Constants for the user options
struct Option
{
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
const std::string img_names[5] = {"sobelDirection.png", "sobelMagnitude.png", "canny.png", "maxsupress.png",
                                  "gauss.png"};
/**
 * @brief Alert listener, when some alert is received, it will be printed and
 * increased the alert counter
 */
void alert_listener();

/**
 * @brief Emergency listener, when some emergency is received, it will be printed and
 * saved in the emergency log file
 */
void run_emergency_listener();

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
void end_conn();

/**
 * @brief Get the command from the message
 * @param message: the command received from the client
 * @return the command number
 */
int get_command(std::string message);

/**
 * @brief Start the database
 */
void start_db();

/**
 * @brief Signal handler
 */
void signal_handler(int signal);

/**
 * @brief Get the image from the client
 * @param message: the message received from the client
 * @return the image id
 */
int get_image(std::string message, IConnection* server, int clientFd);

/**
 * @brief Send the image to the client
 */
void send_image_file(std::string img_name, IConnection* server, int clientFd);

/**
 * @brief Get the image selected
 */
std::string get_image_selected(IConnection* server, int clientFd);

/**
 * @brief Send the image names to the client
 */
void send_images_names(IConnection* server, int clientFd);
