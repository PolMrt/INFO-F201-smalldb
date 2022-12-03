#include "socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include "queries.hpp"

void send_query_result(int socket, const query_result_t &query_result) {
  char buffer_response[1024];

  if (query_result.status == QUERY_SUCCESS) {
    for (auto student: query_result.students) {
      student_to_str(buffer_response, &student, 1024);
      write(socket, buffer_response, 1024);
    }

    snprintf(buffer_response, 1024, "%d student(s) selected", static_cast<int>(query_result.students.size()));
    write(socket, buffer_response, 1024);

    snprintf(buffer_response, 1024, "%s", RESULT_EN_MARKER.c_str());
    write(socket, buffer_response, 1024);
  }
}