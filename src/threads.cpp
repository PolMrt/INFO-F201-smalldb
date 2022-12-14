#include "threads.hpp"

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <iostream>

#include "sig.hpp"
#include "queries.hpp"
#include "socket.hpp"

using namespace std;

// These are suffixex with "_" to not be duplicate symbol with smalldb.cpp
database_t *share_db_;
bool *server_stopping_;

pthread_t new_client(int new_socket, database_t *db, bool *server_stopping_ptr) {
  sigset_t mask_int, mask_usr1;
  server_stopping_ = server_stopping_ptr;
  share_db_ = db;

  // Block signals while accepting connection
  block_sig(&mask_int, SIGINT);
  block_sig(&mask_usr1, SIGUSR1);

  // Creating the thread
  pthread_t tid;
  pthread_create(&tid, NULL, thread_fct, &new_socket);

  cout << "smalldb: Accepted connection (" + to_string(new_socket) + ")" << endl;

  // Unblock signal in the main thread (here)
  unblock_sig(&mask_int);
  unblock_sig(&mask_usr1);

  return tid;
}

void *thread_fct(void *ptr) {
  int socket = (*(int*)ptr);

  char buffer[1024];
  int read_response;

  while ((read_response = read(socket, buffer, 1024)) > 0) {
    query_result_t query_result;
    query_result.query = buffer;

    parse_and_execute(query_result, share_db_, query_result.query);
    send_query_result(socket, query_result);

    // Reset buffer value to nothing, so request do not overlap
    memset(buffer, 0, 1024);
  }

  if (read_response == 0 && !(&server_stopping_)) {
    cout << "smalldb: Client " + to_string(socket) + " disconnected (normal). Closing connection and thread" << endl;
  } else if (!(&server_stopping_)) {
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