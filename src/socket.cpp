#include "socket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#include "queries.hpp"

using namespace std;

void create_server(int &server_fd, struct sockaddr_in &address) {
  // Create the server
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(28772);

  // int bind_return = static_cast<int>(bind(server_fd, (struct sockaddr *)&address, sizeof(address)));
  // if (bind_return < 0) {
  //   perror("smalldb: error when binding the server\n");
  // }

  bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  if (listen(server_fd, 3) < 0) {
    cout << "smalldb: error when starting listening" << endl;
  }
  
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