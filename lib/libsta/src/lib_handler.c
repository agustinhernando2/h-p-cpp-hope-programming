#include <lib_handler.h>

int read_file(char *filename, char **buffer)
{
    FILE *file;
    // Open a file in read mode
    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "%s:%d: Error opening file\n", __FILE__, __LINE__);
        return 1;
    }
    // Get size of the txt file
    if (fseek(file, 0, SEEK_END))
    {
        fclose(file);
        fprintf(stderr, "%s:%d: Error seeking file\n", __FILE__, __LINE__);
        return 1;
    }
    // Get the size of the file
    long fsize = ftell(file);
    if (fsize == -1)
    {
        fclose(file);
        fprintf(stderr, "%s:%d: Error getting file size\n", __FILE__, __LINE__);
        return 1;
    }
    if (fsize == 0)
    {
        fclose(file);
        fprintf(stderr, "%s:%d: File is empty\n", __FILE__, __LINE__);
        return 1;
    }
    // Check if the file is empty
    if (fseek(file, 0, SEEK_SET))
    {
        fclose(file);
        fprintf(stderr, "%s:%d: Error seeking file\n", __FILE__, __LINE__);
        return 1;
    }
    // Store the content of the file
    char *string = malloc((size_t)fsize + 1);
    if (string == NULL)
    {
        fclose(file);
        fprintf(stderr, "%s:%d: Error allocating memory\n", __FILE__, __LINE__);
        return 1;
    }

    // Read the file content
    size_t bytesRead = fread(string, 1, (size_t)fsize, file);
    if (bytesRead != (size_t)fsize)
    {
        fclose(file);
        free(string);
        fprintf(stderr, "%s:%d: Error reading file\n", __FILE__, __LINE__);
        return 1;
    }

    // Add terminator
    string[(size_t)fsize] = '\0';
    fclose(file);
    // Add return
    *buffer = string;

    return 0;
}
int write_file(char *filename, char *buffer)
{
    if ((buffer == NULL) | (strlen(buffer) == 0))
    {
        fprintf(stderr, "%s:%d: buffer error \n", __FILE__, __LINE__);
        return 0;
    }
    FILE *file;
    // Open a file in write mode
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "%s:%d: Error opening file %s\n", __FILE__, __LINE__, filename);
        return 1;
    }
    // Write the each byte of the buffer to the file
    size_t bytesWritten = fwrite(buffer, 1, strlen(buffer), file);
    if (bytesWritten != strlen(buffer))
    {
        fclose(file);
        fprintf(stderr, "%s:%d: Error writing file\n", __FILE__, __LINE__);
        return 1;
    }

    fclose(file);
    return 0;
}

void free_ptr(char **json_file_buffer)
{
    if (*json_file_buffer != NULL)
    {
        free(*json_file_buffer);
        *json_file_buffer = NULL;
    }
}

void error_handler(const char *error_message, char *file, int line)
{
    fprintf(stderr, "%s:%d: Error: %s", file, line, error_message);
    if (errno != 0)
    {
        fprintf(stderr, " Error number: %d", errno);
    }
    fprintf(stderr, "\n");
}

int generate_log(char *filename, char *timestamp, char *message)
{

    FILE *file;
    file = fopen(filename, "a");
    if (file == NULL)
    {
        error_handler("Error opening file", __FILE__, __LINE__);
        return 1;
    }
    fprintf(file, "%s, %s\n", timestamp, message);
    fclose(file);
    return 0;
}

void set_timestamp(char *timestamp, size_t t_size)
{
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(timestamp, t_size, "%a %b %d %H:%M:%S %Y", time_info);
}
