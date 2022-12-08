#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <string>
#include <netinet/in.h>
#include <string>

#include "queries.hpp"

const std::string RESULT_END_MARKER = "**";

void create_server(int &server_fd, struct sockaddr_in &address);

void send_query_result(int socket, const query_result_t &query_result);

int check_and_exit(int returned_value, std::string error_info);

int check_read_write(int returned_value, std::string error_message, int socket);

#endif  // _SOCKET_HPP
