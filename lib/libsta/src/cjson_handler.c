#include <cjson_handler.h>
#include <lib_handler.h>

void print_cjson(char *cjson_buffer)
{
    char *string = NULL;
    cJSON *cjson_object = cJSON_Parse(cjson_buffer);
    if (cjson_object == NULL)
    {
        error_handler("Error parsing json.", __FILE__, __LINE__);
        return;
    }
    string = cJSON_Print(cjson_object);
    if (string == NULL)
    {
        free_cjson_ptr(&cjson_object);
        error_handler("Error printing json.", __FILE__, __LINE__);
    }
    printf("\n%s\n", string);
    free_ptr(&string);
    free_cjson_ptr(&cjson_object);
}

int get_value_of_key_from_json_string(char *cjson_buffer, char *key, char **ptr_buffer)
{
    if (cjson_buffer == NULL)
    {
        error_handler("Error null char*.", __FILE__, __LINE__);
        return 1;
    }
    cJSON *cjson_object = cJSON_Parse(cjson_buffer);
    if (cjson_object == NULL)
    {
        error_handler("Error null cjson*.", __FILE__, __LINE__);
        return -1;
    }
    if (get_value_of_key_from_json_object(cjson_object, key, ptr_buffer))
    {
        free_cjson_ptr(&cjson_object);
        return 1;
    }
    free_cjson_ptr(&cjson_object);
    return 0;
}

int get_value_of_key_from_json_object(cJSON *cjson_object, char *key, char **ptr_buffer)
{
    char *buffer = NULL;
    cJSON *json_value = cJSON_GetObjectItemCaseSensitive(cjson_object, key);
    if (json_value == NULL)
    {
        return 1;
    }
    // if the value is a string
    if (cJSON_IsString(json_value) && (json_value->valuestring != NULL))
    {
        buffer = (char *)malloc(strlen(json_value->valuestring) + 1);
        if (buffer == NULL)
        {
            error_handler("Error allocating memory.", __FILE__, __LINE__);
            return 1;
        }
        // copy value to buffer
        strcpy(buffer, json_value->valuestring);
        *ptr_buffer = buffer;
        return 0;
    }
    // if the value is a number
    if (cJSON_IsNumber(json_value))
    {
        buffer = (char *)malloc(20);
        if (buffer == NULL)
        {
            error_handler("Error allocating memory.", __FILE__, __LINE__);
            return 1;
        }
        // copy value to buffer
        sprintf(buffer, "%f", json_value->valuedouble);
        *ptr_buffer = buffer;
        return 0;
    }
    // if the value is an object
    if (cJSON_IsObject(json_value))
    {
        // create buffer
        buffer = (char *)malloc(strlen(cJSON_Print(json_value)) + 1);
        if (buffer == NULL)
        {
            error_handler("Error allocating memory.", __FILE__, __LINE__);
            return 1;
        }
        // copy value to buffer
        strcpy(buffer, cJSON_Print(json_value));
        *ptr_buffer = buffer;
        return 0;
    }
    return 1;
}

int cjson_add_key_value_to_json_string(char *cjson_buffer, char *key, char *buffer, int flags)
{
    cJSON *cjson_object = NULL;
    cJSON *value = NULL;

    if (buffer == NULL || key == NULL)
    {
        error_handler("Error params.", __FILE__, __LINE__);
        return 1;
    }

    // parse json string or create a new object
    cjson_object = cJSON_Parse(cjson_buffer);
    if (cjson_object == NULL)
    {
        cjson_object = cJSON_CreateObject();
        if (cjson_object == NULL)
        {
            error_handler("Error: Unable to create JSON object.", __FILE__, __LINE__);
            return 1;
        }
    }

    // check if buffer is a number
    if ((flags & INTPARSE) && atof(buffer) != 0)
    {
        value = cJSON_CreateNumber(atof(buffer));
        if (value == NULL)
        {
            free_cjson_ptr(&cjson_object);
            error_handler("Error: Unable to create JSON number value.", __FILE__, __LINE__);
            return 1;
        }
    }
    // check if the buffer is an object
    else if (flags & OBJPARSE)
    {
        value = cJSON_Parse(buffer);
        if (value == NULL)
        {
            free_cjson_ptr(&cjson_object);
            error_handler("Error: Unable to create JSON object value.", __FILE__, __LINE__);
            return 1;
        }
    }
    else
    {
        value = cJSON_CreateString(buffer);
        if (value == NULL)
        {
            free_cjson_ptr(&cjson_object);
            error_handler("Error: Unable to create JSON string value.", __FILE__, __LINE__);
            return 1;
        }
    }

    if (is_key_in_json_object(cjson_object, key))
    {
        if (flags & OVERRIDE)
        {
            cJSON_DeleteItemFromObject(cjson_object, key);
        }
        else
        {
            free_cjson_ptr(&cjson_object);
            free_cjson_ptr(&value);
            error_handler("Error: The key already exists.", __FILE__, __LINE__);
            return 0;
        }
    }
    cJSON_AddItemToObject(cjson_object, key, value);

    if (json_object_to_json_string(cjson_object, cjson_buffer))
    {
        free_cjson_ptr(&cjson_object);
        error_handler("Error: Unable to convert JSON object to string.", __FILE__, __LINE__);
        return 1;
    }
    // value is already in the object, the free is not necessary
    free_cjson_ptr(&cjson_object);
    return 0;
}

int json_object_to_json_string(cJSON *cjson_object, char *buffer)
{
    if (buffer == NULL)
    {
        error_handler("Error: buffer is NULL", __FILE__, __LINE__);
        return 1;
    }

    char *result = cJSON_Print(cjson_object);
    if (result == NULL)
    {
        error_handler("Error getting char buffer json.", __FILE__, __LINE__);
        return 1;
    }

    strcpy(buffer, result);
    free_ptr(&result);
    return 0;
}

int is_key_in_json_object(cJSON *cjson_object, char *key)
{
    cJSON *json_value = cJSON_GetObjectItemCaseSensitive(cjson_object, key);
    if (json_value == NULL)
    {
        return 0;
    }
    return 1;
}

int is_key_in_json_buffer(char *cjson_buffer, char *key)
{
    if (cjson_buffer == NULL || key == NULL)
    {
        error_handler("Error params.", __FILE__, __LINE__);
        return 0;
    }

    // parse json string or create a new object
    cJSON *cjson_object = cJSON_Parse(cjson_buffer);
    if (cjson_object == NULL)
    {
        error_handler("Error: cJSON_Parse.", __FILE__, __LINE__);
        return 0;
    }
    // check if the key is in the object
    if (is_key_in_json_object(cjson_object, key))
    {
        free_cjson_ptr(&cjson_object);
        return 1;
    }
    else
    {
        free_cjson_ptr(&cjson_object);
        return 0;
    }
}

int merge_json_strings(char *json_string1, char *json_string2, char *merged_json)
{
    if (json_string1 == NULL || json_string2 == NULL || merged_json == NULL)
    {
        error_handler("Error null char*.", __FILE__, __LINE__);
        return 1;
    }

    cJSON *json_object1 = cJSON_Parse(json_string1);
    cJSON *json_object2 = cJSON_Parse(json_string2);

    if (json_object1 == NULL || json_object2 == NULL)
    {
        error_handler("Error parsing json objects.", __FILE__, __LINE__);
        return 1;
    }

    cJSON *merged_object = cJSON_Duplicate(json_object1, 1);

    if (merged_object == NULL)
    {
        error_handler("Error duplicating json object.", __FILE__, __LINE__);
        return 1;
    }

    cJSON *current_item = NULL;
    /* Macro for iterating over an array or object */
    cJSON_ArrayForEach(current_item, json_object2)
    {
        // check if the key is already in the merged object
        if (is_key_in_json_object(merged_object, current_item->string))
        {
            cJSON_DeleteItemFromObject(merged_object, current_item->string);
        }
        cJSON_AddItemToObject(merged_object, current_item->string, cJSON_Duplicate(current_item, 1));
    }

    char *merged_string = cJSON_Print(merged_object);

    if (merged_string == NULL)
    {
        error_handler("Error printing merged json.", __FILE__, __LINE__);
        free_cjson_ptr(&merged_object);
        return 1;
    }

    strcpy(merged_json, merged_string);

    free_cjson_ptr(&merged_object);
    free(merged_string);

    return 0;
}

void free_cjson_ptr(cJSON **json)
{
    if (*json != NULL)
    {
        cJSON_Delete(*json);
        *json = NULL;
    }
}
