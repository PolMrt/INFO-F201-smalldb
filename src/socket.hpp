#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <string>
#include <netinet/in.h>
#include <string>

#include "queries.hpp"

// This is used to mark that all results have been sent to the client
const std::string RESULT_END_MARKER = "POLMARNETTEROMAINMARKOWITCH";

/**
 * @brief Create and start the server
 * 
 * @param server_fd will be editeted with the server file descriptor
 * @param address the address used for the server
 */
void create_server(int &server_fd, struct sockaddr_in &address);

/**
 * @brief Send the query result to the client
 * 
 * @param socket the socket's fd
 * @param query_result the query result to send
 */
void send_query_result(int socket, const query_result_t &query_result);

/**
 * @brief Check if the returned value is < 0, if so exit and print an error
 * 
 * @param returned_value the value that will be checked
 * @param error_message the error message
 * @return int : if the value >= 0, it will return it
 */
int check_and_exit(int returned_value, std::string error_message);

/**
 * @brief Check if returned value is <= 0, if so close the socket and print an error, will not exit
 * 
 * @param returned_value the value that will be checked
 * @param error_message the error message
 * @param socket the socket of the client
 * @return int : if the value > 0, it will return it
 */
int check_read_write(int returned_value, std::string error_message, int socket);

#endif  // _SOCKET_HPP
