#include "socket.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "queries.hpp"

void create_server(int &server_fd, struct sockaddr_in &address) {
  check_and_exit((server_fd = socket(AF_INET, SOCK_STREAM, 0)), "creation of the server");

  int opt = 1;
  // The operator | does not work on mac here, cf https://stackoverflow.com/q/58599070/7905936#comment103510725_58599070
  check_and_exit(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "smalldb: error: setting the option of fthe socket (with IP reused)");
  check_and_exit(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "smalldb: error: setting the option of fthe socket (with Port reused)");
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(28772);
  
  check_and_exit(bind(server_fd, (struct sockaddr *)&address, sizeof(address)), "smalldb: error: binding the server");

  check_and_exit(listen(server_fd, 3), "smalldb: error: server listen");
}

void send_query_result(int socket, const query_result_t &query_result) {
  std::string error_message_write = "smalldb: error: an occured while sending data to the client (" + std::to_string(socket) + ")";
  char buffer_response[1024];

  if (query_result.status == QUERY_SUCCESS) {
    if (query_result.type == QUERY_SELECT || query_result.type == QUERY_INSERT) {
      // If select or insert, send students to client
      for (auto student: query_result.students) {
        student_to_str(buffer_response, &student, 1024);
        check_read_write(write(socket, buffer_response, 1024), error_message_write, socket);
      }
    }

    if (query_result.type != QUERY_INSERT) {
      // If not insert, show a nb of students impacted by the request
      snprintf(buffer_response, 1024, "%d student(s) %s", static_cast<int>(query_result.students.size()), get_query_type(query_result).c_str());
      check_read_write(write(socket, buffer_response, 1024), error_message_write, socket);
    }
  } else {
    check_read_write(write(socket, query_result.error_message, sizeof(query_result.error_message)), error_message_write, socket);
  }

  // Send the result end marker which marks the end of the result
  snprintf(buffer_response, 1024, "%s", RESULT_END_MARKER.c_str());
  check_read_write(write(socket, buffer_response, 1024), error_message_write, socket);
}

int check_and_exit(int returned_value, std::string error_info) {
  if (returned_value < 0) {
    perror(error_info.c_str());
    exit(EXIT_FAILURE);
  } else {
    return returned_value;
  }
}

int check_read_write(int returned_value, std::string error_message, int socket) {
  if (returned_value <= 0) {
    perror(error_message.c_str());
    close(socket);
  }
  return returned_value;
}