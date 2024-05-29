#include "file_comunication_handler.hpp"
#include <netinet/in.h>

std::vector<char> receive_vector(int sockfd, std::string output_filename, unsigned long compressedSize)
{
    std::vector<char> file_data; // Vector to save the compressed data`
    ssize_t bytesRead;           // Bytes read
    char buffer[BUFFER_SIZE];    // Buffer

    uint32_t dataSize; // Data size
    if (read(sockfd, &dataSize, sizeof(dataSize)) < 0)
    { // Read the data size
        std::cerr << "ERROR reading data size from socket" << std::endl;
    }
    std::cout << "Data size: " << dataSize << std::endl;
    dataSize = ntohl(dataSize); // Convert to network byte order

    file_data.reserve(dataSize); // Reserve the size of the data

    while (file_data.size() < dataSize && (bytesRead = read(sockfd, buffer, BUFFER_SIZE)) > 0)
    { // Read the data
        file_data.insert(file_data.end(), buffer, buffer + bytesRead);
    }
    if (bytesRead < 0)
    {
        throw std::runtime_error("ERROR reading from socket");
    }

    std::cout << "Received compressed data successfully!" << std::endl;
    write_file(output_filename, file_data, compressedSize);

    std::string message = "Received compressed data successfully! Process id: " + std::to_string(getpid()) + " " +
                          std::to_string(dataSize) + " bytes" + " Original size: " + std::to_string(compressedSize) +
                          " bytes" + " Compression ratio: " + std::to_string((double)compressedSize / dataSize);
    generate_log(message);
    return file_data;
}

void decompressFile(std::vector<char> file_data, std::string output_filename)
{
    // Decompress data
    std::vector<char> decompressedData(file_data.size() *
                                       10);            // Guess the decompressed size as 10 times the compressed size
    uLongf decompressedSize = decompressedData.size(); // Decompressed size
    if (uncompress(reinterpret_cast<Bytef *>(decompressedData.data()), &decompressedSize, // Uncompress the data
                   reinterpret_cast<const Bytef *>(file_data.data()), file_data.size()) != Z_OK)
    {
        throw std::runtime_error("Decompression failed!");
    }
    printf("Decompressed size: %lu bytes\n", decompressedSize);
    // Write decompressed data to file
    write_file(output_filename, decompressedData, decompressedSize);

    std::string message = "Decompressed data successfully! Decompressed size: Process id" + std::to_string(getpid()) +
                          " " + std::to_string(decompressedSize) + " bytes" +
                          " Original size: " + std::to_string(file_data.size()) + " bytes" +
                          " Compression ratio: " + std::to_string((double)file_data.size() / (double)decompressedSize);
    generate_log(message);
}

std::vector<char> compressFile(const std::string &input_filename, const std::string &compressed_filename,
                               int *original_size, unsigned long *compressed_size)
{

    // Verifica si el archivo existe
    if (!std::filesystem::exists(input_filename))
    {
        throw std::runtime_error("Input file does not exist!");
    }

    // Read input file in binary mode
    std::ifstream inputFile(input_filename, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open())
    {
        throw std::runtime_error("Failed to open input file!");
    }
    // Get input file size
    std::streamsize inputSize = inputFile.tellg(); // Get the size of the file
    inputFile.seekg(0, std::ios::beg);             // Move the cursor to the beginning of the file
    std::vector<char> inputData(inputSize);        // Create a vector to store the file data
    if (!inputFile.read(inputData.data(), inputSize))
    {
        inputFile.close();
        throw std::runtime_error("Failed to read input file!");
    }
    inputFile.close();

    // Compress input data
    unsigned long compressedSize = compressBound(inputSize);
    std::vector<char> compressedData(compressedSize); // Create a vector to store the compressed data
    if (compress(reinterpret_cast<Bytef *>(compressedData.data()), &compressedSize,
                 reinterpret_cast<const Bytef *>(inputData.data()), inputSize) != Z_OK)
    {
        throw std::runtime_error("Compression failed!");
    }

    write_file(compressed_filename, compressedData, compressedSize);

    *original_size = (int)inputSize;
    *compressed_size = compressedSize;
    return compressedData;
}

void write_file(const std::string &compressed_filename, std::vector<char> &data, unsigned long size)
{
    // Write compressed data to file
    std::ofstream file(compressed_filename, std::ios::binary);
    if (!file.write(data.data(), size))
    {
        file.close();
        throw std::runtime_error("Failed to write compressed data to file!");
    }
    file.close();
}

void send_vector(int sockfd, std::vector<char> file_data, unsigned long compressedSize)
{
    uint32_t dataSize = htonl((uint32_t)compressedSize); // Convertir a formato de red
    if (write(sockfd, &dataSize, sizeof(dataSize)) < 0)
    { // Send the data size
        throw std::runtime_error("ERROR writing data size to socket");
    }
    if (write(sockfd, file_data.data(), compressedSize) < 0)
    { // Send the data
        throw std::runtime_error("ERROR writing data to socket");
    }
}

std::string get_current_timestamp()
{
    // Obtener el tiempo actual
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Convertir el tiempo a una estructura tm
    std::tm now_tm;
    localtime_r(&now_time, &now_tm);

    // Crear una cadena de formato de tiempo
    std::ostringstream timestamp;
    timestamp << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

    return timestamp.str();
}

int generate_log(const std::string &message)
{
    std::ofstream file;
    file.open(logfilename, std::ios_base::app); // Open in append mode
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << logfilename << " in " << __FILE__ << " at line " << __LINE__
                  << std::endl;
        return 1;
    }
    file << get_current_timestamp() << ", " << message << std::endl;
    file.close();
    return 0;
}
