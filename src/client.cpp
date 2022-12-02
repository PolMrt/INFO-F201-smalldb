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

  // Conversion de string vers IPv4 ou IPv6 en binaire
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  
  // char buffer[1024];
  // int longueur, i, ret;

  std::string request;
  
  while (std::getline(std::cin, request)) {
    query_result_t *query_result;

      printf("Envoi...\n");
      write(sock, request.c_str(), request.length());

      int read_response = read(sock, query_result, sizeof(query_result_t));
      std::cout << read_response << std::endl;

    //  for (auto res: query_result->students) {
    //   std::cout << res.fname << std::endl;
    //  }
  }
  
  close(sock);
  return 0;
}
