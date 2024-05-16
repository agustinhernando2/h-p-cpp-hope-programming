/*preventing multiple inclusions of the same header file*/
#pragma once

/* Unix libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Libraries*/
#include <cJSON.h>
#include <lib_handler.h>

#define INTPARSE 4
#define OBJPARSE 8
#define OVERRIDE 1

/**
 * @brief Print a JSON character buffer
 *  verify if the buffer is a json object and print it.
 * @param [in] buffer JSON character buffer to print.
 */
void print_cjson(char *buffer);

/**
 * @brief Add key-value pair to a JSON object.
 *
 * This function adds a key-value pair to a JSON object.
 *
 * @param cjson_object Pointer to the JSON object to which the key-value pair will be added.
 * @param key Content of the key.
 * @param value Content of the value.
 * @param cjson_buffer Content of the JSON character buffer result.
 * @param flags OVERRIDE to override is already exist, INTPARSE to try save int value.
 * @return 0 if successful, 1 in case of an error.
 */
int cjson_add_key_item_to_json_object(cJSON *cjson_object, char *key, cJSON *value, char *cjson_buffer, int flags);

/**
 * @brief Add key-value pair to a JSON character buffer.
 *
 * This function adds a key-value pair to a JSON character buffer.
 *
 * @param cjson_buffer Pointer to the JSON character buffer to which the key-value pair will be added.
 * @param key Content of the key.
 * @param buffer Content of the value.
 * @param flags OVERRIDE to override is already exist.
 * @return 0 if successful, 1 in case of an error.
 */
int cjson_add_key_value_to_json_string(char *cjson_buffer, char *key, char *buffer, int flags);

/**
 * @brief - get value of a JSON object key
 *
 * @param cjson_object JSON object.
 * @param key Key to search for in the JSON object.
 * @param ptr_buffer pointer to save the result content.
 * @return 0 if successful, 1 in case of an error.
 */
int get_value_of_key_from_json_object(cJSON *cjson_object, char *key, char **ptr_buffer);

/**
 * @brief - get value of a JSON character buffer
 *
 * @param cjson_buffer JSON character buffer.
 * @param key Key to search for in the JSON.
 * @param ptr_buffer pointer to save the result content.
 * @return 0 if successful, 1 in case of an error.
 */
int get_value_of_key_from_json_string(char *cjson_buffer, char *key, char **ptr_buffer);

/**
 * @brief - parse JSON object to JSON character buffer
 *
 * @param cjson_object JSON object.
 * @param buffer receive an JSON character buffer.
 * @return 1 success. 1 if an error occurs.
 */
int json_object_to_json_string(cJSON *cjson_object, char *buffer);

/**
 * @brief - merge two JSON character buffers
 *
 * @param json_string1 JSON character buffer 1.
 * @param json_string2 JSON character buffer 2.
 * @param merged_json content of the JSON character buffer merged.
 * @return 0 success. 1 if an error occurs.
 */
int merge_json_strings(char *json_string1, char *json_string2, char *merged_json);

/**
 * @brief - check if a key is in a JSON object
 *
 * @param cjson_object cJSON to analize.
 * @return 1 if the key is in the cJSON object, 0 otherwise.
 */
int is_key_in_json_object(cJSON *cjson_object, char *key);

/**
 * @brief - check if a key is in a JSON character buffer
 *
 * @param cjson_buffer JSON character buffer to analize.
 * @return 1 if the key is in the JSON character buffer, 0 otherwise.
 */
int is_key_in_json_buffer(char *cjson_buffer, char *key);

/**
 * @brief Frees the memory allocated
 *
 * @param json: cJSON free.
 */
void free_cjson_ptr(cJSON **json);
