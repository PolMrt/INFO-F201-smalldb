#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#include <string>

#include "queries.hpp"

const std::string RESULT_EN_MARKER = "**";

void send_query_result(int socket, const query_result_t &query_result);

#endif  // _SOCKET_HPP
