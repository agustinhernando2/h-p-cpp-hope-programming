#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <rocksDbWrapper.hpp>

const std::string SUPPLIES_KEY = "supplies";
const std::string SUPPLY_NAME_KEY = "supply_name";
const std::string SUPPLY_AMOUNT_KEY = "supply_amount";
const std::string SUPPLY_TYPE_KEY = "supply_type";

extern const std::string rockdb_path;
/*
* This function sets a new supply in the database
* @param command: the command received from the client
*/
void set_supply(std::string command);

/*
* This function gets all the supplies from the database 
* @return: a string with all the supplies in json format
*/
std::string get_supplies();
