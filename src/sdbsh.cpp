#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "queries.hpp"
#include "socket.hpp"

using namespace std;

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    cout << "sdbsh: error: please provide the ip of the server";
    return 1;
  }
  const char * ip = argv[1];

  int sock = check_and_exit(socket(AF_INET, SOCK_STREAM, 0), "sdbsh: error: cannot connect to the server");
  

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);
  inet_pton(AF_INET, ip, &serv_addr.sin_addr);
  check_and_exit(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)), "sdbsh: error: cannot connect to the server");

  std::string request;
  while (std::getline(std::cin, request)) {
    query_result_t query_result;

    int bytes_write = write(sock, request.c_str(), request.length());

    if (bytes_write < 0) {
      perror("sdbsh: lost connection with the server\n");
      close(sock);
      exit(EXIT_FAILURE);

      // If bytes write == 0, it means the user tried to send nothing
    } else if (bytes_write > 0) {
      char response_buffer[1024];
      ssize_t bytes_read;
      
      while ((bytes_read = read(sock, response_buffer, 1024)) > 0 && strcmp(response_buffer, RESULT_END_MARKER.c_str()) != 0) {
        std::cout << response_buffer << std::endl;
      }
      
      if (bytes_read <= 0) {
        perror("sdbsh: lost connection with the server\n");
        close(sock);
        exit(EXIT_FAILURE);
      }
    }

  }

  return 0;
}
