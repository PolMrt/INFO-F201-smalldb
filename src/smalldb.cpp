#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

#include "sig.hpp"
#include "db.hpp"
#include "queries.hpp"
#include "socket.hpp"

using namespace std;

database_t *share_db;
vector<int> connections_sockets;
bool server_stopping = false;

void signal_handler(int signal) {
  cout << "in sig handler" << endl;
  if (signal == SIGINT) {
    server_stopping = true;
    cout << "smalldb: stopping the server..." << endl;

    // for (int socket_id: connections_sockets) {
    //   cout << "smalldb: Closing connection " + std::to_string(socket_id) << endl;
    //   close(socket_id);
    // }

    // db_save(share_db);
    exit(EXIT_SUCCESS);
  } else {
    return;
  }
}

void *thread_fct(void *ptr) {
  int *socket = (int*)ptr;

  char buffer[1024];
  int read_response;

  while ((read_response = read(*socket, buffer, 1024)) > 0) {
    query_result_t query_result;
    query_result.query = buffer;

    parse_and_execute(query_result, share_db, query_result.query);
    send_query_result(*socket, query_result);

    // Reset buffer value to nothing, so request do not overlap
    memset(buffer, 0, 1024);
  }

  if (read_response == 0 && !server_stopping) {
    cout << "smalldb: Client " + to_string(*socket) + " disconnected (normal). Closing connection and thread" << endl;
  } else if (!server_stopping) {
    cout << "smalldb: Lost connection to client " + to_string(*socket) << endl;
    cout << "smalldb: Closing connection " + to_string(*socket) << endl;
    cout << "smalldb: Closing thread for connection " + to_string(*socket) << endl;
  }
  close(*socket);
  
  return nullptr;
}

int main(int argc, char const *argv[]) {
  // init the db
  share_db = new database_t;

  if (argc < 2) {
    cout << "smalldb: error: please provide a path to the database";
    return 1;
  }

  // load file if provided
  const char *db_path = argv[1];
  db_load(share_db, db_path);

  string db_path_str = "";
  db_path_str.append(db_path);

  cout << "smalldb: DB loaded (" + db_path_str + "): " + to_string(share_db->data.size()) + " students in database" << endl;

  // Register signal handler
  signal(SIGINT, &signal_handler);

  // Create the server
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(28772);

  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3);
  
  size_t addrlen = sizeof(address);
  sigset_t mask;

  while (1) {
    int new_socket = int(accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen));

    block_sig(&mask, SIGINT);

    connections_sockets.push_back(new_socket);
    pthread_t tid;
    pthread_create(&tid, NULL, thread_fct, &new_socket);

    cout << "smalldb: Accepted connection (" + to_string(new_socket) + ")" << endl;

    unblock_sig(&mask);
  }
  
  close(server_fd);

  return 0;
}

