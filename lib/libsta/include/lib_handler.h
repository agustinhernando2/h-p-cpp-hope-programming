#pragma once
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Reads the content of a file and stores it in a buffer.
 *        Allocates memory for the buffer using malloc.
 * @param filename: The name of the file to read.
 * @param buffer: Pointer to a pointer where the read content will be stored.
 * @return 0 on success, 1 if an error occurs.
 */
int read_file(char *filename, char **buffer);

/**
 * @brief Writes the content of a buffer to a file.
 *
 * @param filename: The name of the file to write.
 * @param buffer: Pointer to the buffer containing the bytes to write.
 * @return 0 on success, 1 if an error occurs.
 */
int write_file(char *filename, char *buffer);

/**
 * @brief Frees the memory allocated
 *
 * @param json_file_buffer: Pointer free.
 */
void free_ptr(char **json_file_buffer);

/**
 * @brief Handle error
 *
 * @param error_message: Error message to print.
 * @param error_number: Error number to print.
 */
void error_handler(const char *error_message, char *file, int line);

/**
 * @brief Generate new line log
 */
int generate_log(char *filename, char *timestamp, char *message);

/**
 * @brief Set the timestamp
 */
void set_timestamp(char *timestamp, size_t t_size);
