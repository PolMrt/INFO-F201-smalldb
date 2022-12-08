#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "queries.hpp"
#include "socket.hpp"

using namespace std;

void check_disconnected_rw(size_t value, int sock) {
  if (value <= 0) {
    cout << "sdbsh: lost connection with the server" << endl;
    close(sock);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    cout << "sdbsh: error: please provide the ip of the server";
    return 1;
  }
  const char * ip = argv[1];

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);
  inet_pton(AF_INET, ip, &serv_addr.sin_addr);
  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  std::string request;
  while (std::getline(std::cin, request)) {
    query_result_t query_result;

    int bytes_write = write(sock, request.c_str(), request.length());
    check_disconnected_rw(bytes_write, sock);


    char response_buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(sock, response_buffer, 1024)) > 0 && strcmp(response_buffer, RESULT_END_MARKER.c_str()) != 0) {
      std::cout << response_buffer << std::endl;
    }
    check_disconnected_rw(bytes_read, sock);
  }

  return 0;
}
