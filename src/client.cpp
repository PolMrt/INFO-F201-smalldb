#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);

  // Conversion de string vers IPv4 ou IPv6 en binaire
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  
  char buffer[1024];
  int longueur, i, ret;
  
  if (fgets(buffer, 1024, stdin) != NULL) {
     longueur = strlen(buffer) + 1;
     printf("Envoi...\n");
     write(sock, buffer, strlen(buffer) + 1);
  }
  
  close(sock);
  return 0;
}
