#pragma once
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <cppSocketLib.hpp>
#include <cannyEdgeFilter.hpp>
#include <rocksDbWrapper.hpp>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <thread>

#include <httplib.h>

#define IPV6_ "IPV6"
#define IPV4_ "IPV4"


void alert_listen(int msgid);
int run_server(bool is_ipv6, std::string port);
bool authentication(std::string username, std::string password);
bool check_json_users(const std::string& username, const std::string& password, const std::string& path);
void handleClient(IConnection* server, int clientFd);
void handler_principal(IConnection* server, int clientFd, std::string user);
std::string read_file(const std::string &path);
void start_http_server();
std::string get_supplies();
void supplies_data(IConnection* server, int clientFd, std::string user);
