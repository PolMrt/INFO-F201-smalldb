#ifndef _THREADS_HPP
#define _THREADS_HPP

#include <signal.h>
#include "db.hpp"

void new_client(sigset_t *mask_int, sigset_t *mask_usr1, int new_socket, database_t *share_db, bool *server_stopping);

void *thread_fct(void *ptr);

#endif  // _THREADS_HPP
