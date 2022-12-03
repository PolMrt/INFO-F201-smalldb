#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "queries.hpp"

using namespace std;

int main(void) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  std::string request;
  while (std::getline(std::cin, request)) {
    query_result_t query_result;
    printf("Envoi...\n");
    cout << request << endl;

    write(sock, request.c_str(), request.length());

    char response_buffer[1024];
    ssize_t bytes_read;
    while (bytes_read = read(sock, response_buffer, 1024) > 0 && strcmp(response_buffer, "**") != 0) {
      std::cout << response_buffer << std::endl;
    }
  }
  
  close(sock);
  return 0;
}
