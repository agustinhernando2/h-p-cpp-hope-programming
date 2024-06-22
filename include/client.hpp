#pragma once
#include <arpa/inet.h>
// #include <boost/algorithm/string.hpp>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <zip.h>

// include opencv
#include <cppSocketLib.hpp>
#include <file_comunication_handler.hpp>
#include <nlohmann/json.hpp>

/**
 * @brief Structure to store the options
 */
struct Option
{
    /**
     * @brief Message type
     */
    int number;
    /**
     * @brief Command to send
     */
    std::string command;
    /**
     * @brief Description of the command
     */
    std::string description;
};

const Option option1 = {1, "get_supplies", "Get supplies from server"};
const Option option2 = {2, "set_supplies", "Set supplies to server"};
const Option option3 = {3, "img_filtering", "Send image to process"};
const Option option4 = {4, "get_img_filtered", "Get image filtered from server"};
const Option option5 = {5, "end_conn", "End Connection"};

// Constants for user
const char* ADMIN_USER = "admin";
const char* ADMIN_PASS = "admin";
const int ADMIN_LEVEL = 1;
const int NORMAL_LEVEL = 0;

// Path to save the images
const std::string img_path = "img_cli/";

// Localhost IP
// const char* localhost_ipv4 = "127.0.0.1";
// const char* localhost_ipv6 = "::1";
// Localhost ports
const char* tcp4_port = "8080";
const char* tcp6_port = "8081";

// flag to option 1 in loop
int flag_opt1 = 0;

/**
 * @brief Class to store the user credentials
 */
class UserCredentials
{
  private:
    std::string username;
    std::string password;

  public:
    /**
     * @brief get the user credentials
     */
    void get_credentials();
    /**
     * @brief get the user level
     */
    int get_user_level();
    /**
     * @brief get the user name
     */
    std::string get_username() const; // const member function, it does not modify the object
    /**
     * @brief get the user password
     */
    std::string get_password() const; // const member function, it does not modify the object
};

/**
 * @brief Clear the screen
 */
void clear_screen();

/**
 * @brief Get options from the user
 */
int get_options();

/**
 * @brief Get image name from the response
 */
std::string get_img_name(std::string response);

/**
 * @brief End the client connection
 */
void end_client_conn();

/**
 * @brief Signal handler
 */
void signal_handler(int signal);

/**
 * @brief Send the get supplies command
 */
void send_get_options_image_filtered_command(UserCredentials user, int serverSocket, std::unique_ptr<IConnection>& con);

/**
 * @brief Send the get image filtered command
 */
void send_get_image_filtered_command(UserCredentials user, std::string img_name, int serverSocket,
                                     std::unique_ptr<IConnection>& con);

/**
 * @brief Send the get supplies command
 * @param user to append credentials
 * @param serverSocket to send the command
 * @param con to send the command
 */
void send_get_supplies_command(UserCredentials user, int serverSocket, std::unique_ptr<IConnection>& con);

/**
 * @brief Send the image filtering command
 * @param user to append credentials
 * @param img_name to append the image name
 * @param nproc to append the number of processes that will be used
 * @param serverSocket to send the command
 * @param con to send the command
 */
void send_image_filtering_command(UserCredentials user, std::string img_name, int nproc, int serverSocket,
                                  std::unique_ptr<IConnection>& con);

/**
 * @brief Send the set supplies command
 * @param user
 * @param serverSocket
 */
void send_set_supplies_command(UserCredentials user, std::string sup_type, std::string sup_name, std::string sup_amount,
                               int serverSocket, std::unique_ptr<IConnection>& con);
