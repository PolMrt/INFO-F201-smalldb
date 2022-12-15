#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

#include "db.hpp"
#include "queries.hpp"
#include "threads.hpp"
#include "socket.hpp"

using namespace std;

int server_fd;
vector< pair<int *, pthread_t *> > connections_sockets_threads;
database_t *share_db;
bool server_stopping = false;

void signal_handler(int signal) {
  if (signal == SIGINT) {
    server_stopping = true;

    cout << endl; // Break line after ^C
    cout << "smalldb: stopping the server..." << endl;

    for (auto socket_threads: connections_sockets_threads) {
      // Try to close to socket to see if the conenction is still live
      // close returns 0 when it was successfull
      int closed = close(*socket_threads.first);
      if (closed == 0) {
        cout << "smalldb: Closing connection " + to_string(*socket_threads.first) << endl;
        cout << "smalldb: Closing thread for connection " + to_string(*socket_threads.first) << endl;
      }
      pthread_join(*socket_threads.second, nullptr);
    }
    db_save(share_db);
    close(server_fd);

    // Delete pointer inside the vector
    for (auto socket_threads: connections_sockets_threads) {
      delete socket_threads.first;
      delete socket_threads.second;
    }

    exit(EXIT_SUCCESS);
  } else if (signal == SIGUSR1) {
    cout << "smalldb: saving the database" << endl;
    db_save(share_db);
  } else {
    return;
  }
}

int main(int argc, char const *argv[]) {
  // init the db
  share_db = new database_t;

  if (argc < 2) {
    cout << "smalldb: error: please provide a path to the database" << endl;
    return 1;
  }

  // load file if provided
  const char *db_path = argv[1];
  db_load(share_db, db_path);

  // Register signal handler
  struct sigaction action;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  // Define signal handler for SIGINT
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGUSR1, &action, NULL);

  struct sockaddr_in address;

  create_server(server_fd, address);
  
  size_t addrlen = sizeof(address);

  while (1) {
    int *new_socket = new int(accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen));
    if (*new_socket > 0) {
      // The connection to the client was successfull, now we create its thread
      // Save the file descriptor & thread id in a vector
      pthread_t *tid = new pthread_t(new_client(new_socket, share_db, &server_stopping));
      connections_sockets_threads.push_back(make_pair(new_socket, tid));
    } else {
      // An error occured while accepting socket
      if (errno != EINTR) {
        perror("smalldb: an unexpected error occured while accepting a new connection");
      }
      // If the errno is EINTR (Interrupted system call), it meand the accept was interrupted
      // by SIGUSR1. There's nothing to do in that case, only if we are not in that case.
    }
  }
  close(server_fd);

  return 0;
}

