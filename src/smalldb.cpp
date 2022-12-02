#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

void *threadFct(void *ptr) {
  int *socket = (int*)ptr;

  char buffer[1024];
  int read_response = read(*socket, buffer, 1024);

  if (read_response > 0) {
    printf("Message reçu:%s\n", buffer);
  } else if (read_response == 0) {
    cout << "smalldb: Client " + to_string(*socket) + " disconnected (normal). Closing connection and thread" << endl;
  } else {
    cout << "smalldb: Lost connection to client " + to_string(*socket) << endl;
  }
  close(*socket);
}

int main() {
  vector<int*> connections_sockets;
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(28772);

  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3); // Max 3 connection waiting

  size_t addrlen = sizeof(address);
  
  while (1) {
    int *new_socket = new int(accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen));

    connections_sockets.push_back(new_socket);
    pthread_t tid;
    pthread_create(&tid, NULL, threadFct, new_socket);

    cout << "smalldb: Accepted connection (" + to_string(*new_socket) + ")" << endl;
  }
  
  close(server_fd);

  return 0;
}

