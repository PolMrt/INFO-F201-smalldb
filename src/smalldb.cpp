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
int server_fd;
bool server_stopping = false;

void signal_handler(int signal) {
  if (signal == SIGINT) {
    server_stopping = true;

    cout << endl; // Break line after ^C
    cout << "smalldb: stopping the server..." << endl;

    for (int socket_id: connections_sockets) {
      close(socket_id);
    }

    db_save(share_db);
    close(server_fd);
    exit(EXIT_SUCCESS);
  } else if (signal == SIGUSR1) {
    cout << "smalldb: saving the database" << endl;
    db_save(share_db);
  } else {
    return;
  }
}

void *thread_fct(void *ptr) {
  int socket = (*(int*)ptr);

  char buffer[1024];
  int read_response;

  while ((read_response = read(socket, buffer, 1024)) > 0) {
    query_result_t query_result;
    query_result.query = buffer;

    parse_and_execute(query_result, share_db, query_result.query);
    send_query_result(socket, query_result);

    // Reset buffer value to nothing, so request do not overlap
    memset(buffer, 0, 1024);
  }

  if (read_response == 0 && !server_stopping) {
    cout << "smalldb: Client " + to_string(socket) + " disconnected (normal). Closing connection and thread" << endl;
  } else if (!server_stopping) {
    cout << "smalldb: Lost connection to client " + to_string(socket) << endl;
    cout << "smalldb: Closing connection " + to_string(socket) << endl;
    cout << "smalldb: Closing thread for connection " + to_string(socket) << endl;
  } else {
    cout << "smalldb: Closing connection " + to_string(socket) << endl;
    cout << "smalldb: Closing thread for connection " + to_string(socket) << endl;
  }


  close(socket);
  return nullptr;
}


void new_client(sigset_t *mask_int,sigset_t *mask_usr1, int new_socket){
  // Block signals while accepting connection
  block_sig(mask_int, SIGINT);
  block_sig(mask_usr1, SIGUSR1);
  
  // Save the file descriptor in a vector
  connections_sockets.push_back(new_socket);

  // Creating the thread
  pthread_t tid;
  pthread_create(&tid, NULL, thread_fct, &new_socket);

  cout << "smalldb: Accepted connection (" + to_string(new_socket) + ")" << endl;

  // Unblock signal in the main thread (here)
  unblock_sig(mask_int);
  unblock_sig(mask_usr1);
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
  sigset_t mask_int, mask_usr1;

  while (1) {
    int new_socket = int(accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen));
    if (new_socket > 0) {
      // Accepting the socket was successfull
      new_client(&mask_int, &mask_usr1, new_socket);
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

