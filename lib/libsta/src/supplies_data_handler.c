#include <supplies_data_handler.h>

int get_supply_status(char **supplies_buffer)
{
    return read_file(JSON_FILE_PATH, supplies_buffer);
}

int set_supply_status(char *category, char *key, char *value, char **supplies_buffer)
{
    char *value_of_supply_key = NULL;
    char *value_of_category = NULL;

    if (read_file(JSON_FILE_PATH, supplies_buffer))
    {
        return 1;
    }
    // get the value of the key supplies, ex {"supplies":{...}}
    if (get_value_of_key_from_json_string(*supplies_buffer, SUPPLIES, &value_of_supply_key))
    {
        return 1;
    }
    // get the value of the key category, ex {"food":{...}}
    if (get_value_of_key_from_json_string(value_of_supply_key, category, &value_of_category))
    {
        return 1;
    }
    // add the key-value pair to the category object, ex {"food":{"water":120 ...}} and override if already exist
    if (cjson_add_key_value_to_json_string(value_of_category, key, value, OVERRIDE | INTPARSE))
    {
        return 1;
    }
    // update the category object
    if (cjson_add_key_value_to_json_string(value_of_supply_key, category, value_of_category, OVERRIDE | OBJPARSE))
    {
        return 1;
    }
    // update the supplies object
    if (cjson_add_key_value_to_json_string(*supplies_buffer, SUPPLIES, value_of_supply_key, OVERRIDE | OBJPARSE))
    {
        return 1;
    }
    // write the updated supplies object to the file
    return write_file(JSON_FILE_PATH, *supplies_buffer);
}
