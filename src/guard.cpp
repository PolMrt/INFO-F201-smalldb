#include "guard.hpp"

#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t new_access = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_access = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reader_registration = PTHREAD_MUTEX_INITIALIZER;
int readers_count = 0;

void write_guard_before() {
  pthread_mutex_lock(&new_access);
  pthread_mutex_lock(&write_access);
  pthread_mutex_unlock(&new_access);
}

void write_guard_after() {
  pthread_mutex_unlock(&write_access);
}

void read_guard_before() {
  pthread_mutex_lock(&new_access);
  pthread_mutex_lock(&reader_registration);
  if (readers_count == 0) pthread_mutex_lock(&write_access);
  readers_count++;
  pthread_mutex_unlock(&new_access);
  pthread_mutex_unlock(&reader_registration);
}

void read_guard_after() {
  pthread_mutex_lock(&reader_registration);
  readers_count--;
  if (readers_count == 0) pthread_mutex_unlock(&write_access);
  pthread_mutex_unlock(&reader_registration);
}