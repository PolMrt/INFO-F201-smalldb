#include "socket.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "queries.hpp"

void create_server(int &server_fd, struct sockaddr_in &address) {
  check((server_fd = socket(AF_INET, SOCK_STREAM, 0)), "creation of the server");

  int opt = 1;
  // The operator | does not work on mac here, cf https://stackoverflow.com/q/58599070/7905936#comment103510725_58599070
  check(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setting the option of fthe socket (with IP reused)");
  check(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "setting the option of fthe socket (with Port reused)");
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(28772);

  check(bind(server_fd, (struct sockaddr *)&address, sizeof(address)), "binding the server");

  check(listen(server_fd, 3), "server listen");
}

void send_query_result(int socket, const query_result_t &query_result) {
  char buffer_response[1024];

  if (query_result.status == QUERY_SUCCESS) {
    if (query_result.type == QUERY_SELECT || query_result.type == QUERY_INSERT) {
      for (auto student: query_result.students) {
        student_to_str(buffer_response, &student, 1024);
        write(socket, buffer_response, 1024);
      }
    }

    if (query_result.type != QUERY_INSERT) {
      snprintf(buffer_response, 1024, "%d student(s) %s", static_cast<int>(query_result.students.size()), get_query_type(query_result).c_str());
      write(socket, buffer_response, 1024);
    }
  } else {
    write(socket, query_result.error_message, sizeof(query_result.error_message));
  }

  snprintf(buffer_response, 1024, "%s", RESULT_EN_MARKER.c_str());
  write(socket, buffer_response, 1024);
}

int check(int returned_value, std::string error_info) {
  if (returned_value < 0) {
    std::string error_message = "smalldb: error occured [" + error_info + "]";
    perror(error_message.c_str());
    exit(EXIT_FAILURE);
  } else {
    return returned_value;
  }
}