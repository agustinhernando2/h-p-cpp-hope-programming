#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <filesystem>

const int BUFFER_SIZE = 4096; // Buffer size to receive data
const std::string extension = ".gz"; // Extension of the compressed files
const std::string logfilename = "/var/log/refuge.log"; 

/**
 * @brief Function to receive a vector from a socket
 * @param sockfd Socket file descriptor
 * @param output_filename Name of the file to save the data
 * @param compressedSize Size of the compressed data
 * @return std::vector<char> Vector with the data received
 * @throw std::runtime_error If there is an error reading from the socket
*/
std::vector<char> receive_vector(int sockfd, std::string output_filename, unsigned long compressedSize);

/**
 * @brief Function to send a vector through a socket
 * @param sockfd Socket file descriptor
 * @param file_data Vector to send
 * @param compressedSize Size of the compressed data
 * @return void
*/
void send_vector(int sockfd, std::vector<char> file_data, unsigned long compressedSize);

/**
 * @brief Function to decompress a file
 * @param file_data Vector with the compressed data
 * @param output_filename Name of the file to save the decompressed data
 * @return void
 * @throw std::runtime_error If the decompression fails
*/
void decompressFile(std::vector<char> file_data, std::string output_filename);

/**
 * @brief Function to compress a file
 * @param input_filename Name of the file to compress
 * @param compressed_filename Name of the file to save the compressed data
 * @param original_size Pointer to store the original size of the file
 * @param compressed_size Pointer to store the compressed size of the file
 * @return std::vector<char> Vector with the compressed data
 * @throw std::runtime_error If there is an error reading the input file
*/
std::vector<char> compressFile(const std::string& input_filename, const std::string& compressed_filename, int* original_size, unsigned long* compressed_size);

/**
 * @brief Function to write a vector to a file
 * @param compressed_filename Name of the file to save the data
 * @param data Vector with the data to write
 * @param size Size of the data
 * @return void
*/
void write_file(const std::string& compressed_filename, std::vector<char>& data, unsigned long size);

/**
 * @brief Function to generate a log
 * @param filename Name of the file to save the log
 * @param timestamp Timestamp of the log
*/
int generate_log(const std::string& message);

/**
 * @brief Function to get the current timestamp
*/
std::string get_current_timestamp();
