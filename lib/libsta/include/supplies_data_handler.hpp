#pragma once

#include <file_comunication_handler.hpp>
#include <nlohmann/json.hpp>
#include <rocksDbWrapper.hpp>
#include <string>

const std::string SUPPLIES_KEY = "supplies";
const std::string SUPPLY_NAME_KEY = "supply_name";
const std::string SUPPLY_AMOUNT_KEY = "supply_amount";
const std::string SUPPLY_TYPE_KEY = "supply_type";

extern const std::string database;

/*
 * This function sets a new supply in the database
 * @param command: the command received from the client
 */
void set_supply(std::string command);

/*
 * This function sets a new supply in the database
 * @param message: the message received from the client
 * @param database: the path to the database
 */
void set_supply_from_database(std::string message, std::string database);

/*
 * This function gets all the supplies from the database
 * @return: a string with all the supplies in json format
 */
std::string get_supplies();

/*
 * This function gets all the supplies from the database
 * @param database: the path to the database
 * @return: a string with all the supplies in json format
 */
std::string get_supplies_from_database(std::string database);
