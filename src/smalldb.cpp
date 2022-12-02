#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

void *threadFct(void *ptr) {
  int *socket = (int*)ptr;

  printf("in thread\n");
  char buffer[1024];
  read(*socket, buffer, 1024); printf("Message reçu:%s\n", buffer);
  close(*socket);
}

int main() {
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
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    pthread_t tid;
    pthread_create(&tid, NULL, threadFct, &new_socket);
  }
  
  close(server_fd);

  return 0;
}

