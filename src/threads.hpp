#ifndef _THREADS_HPP
#define _THREADS_HPP

#include <pthread.h>
#include <signal.h>

#include "db.hpp"

/**
 * @brief to be called when a new client is create, will manage the signal blocking and create the thread
 * 
 * @param new_socket the fd of the new socket
 * @param db a pointer to the shared_db
 * @param server_stopping_ptr a pointer to the boolean saying wheter the server is stopping or not
 * @return pthread_t the id of the created thread
 */
pthread_t new_client(int *new_socket, database_t *db, bool *server_stopping_ptr);

/**
 * @brief The function that will be executed by each thread
 */
void *thread_fct(void *ptr);

#endif  // _THREADS_HPP
