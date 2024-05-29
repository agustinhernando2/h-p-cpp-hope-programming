#include <supplies_data_handler.hpp>

void set_supply(std::string message){
    set_supply_from_database(message, database);
}
void set_supply_from_database(std::string message, std::string database)
{   
    // Open the database or create it if it does not exist
    std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);

    nlohmann::json new_item;
    nlohmann::json command_json = nlohmann::json::parse(message);
    
    // Get values as strings
    std::string supply_name = command_json[SUPPLY_NAME_KEY].get<std::string>();
    std::string supply_amount = command_json[SUPPLY_AMOUNT_KEY].get<std::string>();
    std::string supply_type = command_json[SUPPLY_TYPE_KEY].get<std::string>();
    
    std::string supplies;

    db->get(SUPPLIES_KEY, supplies);
    if(supplies.empty())
    {
        supplies = "{}";
    }
    // Load the current supplies in the json object
    new_item = nlohmann::json::parse(supplies);
    // Add the new supply in number format
    new_item[supply_type][supply_name] = std::stoi(supply_amount);
    // Update the database
    db->put(SUPPLIES_KEY, new_item.dump());

}

std::string get_supplies(){
    return get_supplies_from_database(database);
}

std::string get_supplies_from_database(std::string database)
{   
    std::string supplies;
    std::unique_ptr<RocksDbWrapper> db = std::make_unique<RocksDbWrapper>(database);
    nlohmann::json j;

    // Get all supplies
    db->get(SUPPLIES_KEY, supplies);
    j[SUPPLIES_KEY] = nlohmann::json::parse(supplies);
    
    // Format the json
    supplies = j[SUPPLIES_KEY].dump(4);
    return supplies;
}