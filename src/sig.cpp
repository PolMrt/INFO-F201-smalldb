#include "sig.hpp"

#include <signal.h>
#include <unistd.h>
#include <pthread.h>

void block_sig(sigset_t *mask, int signal) {
  sigemptyset(mask);
  sigaddset(mask, signal);
  sigprocmask(SIG_BLOCK, mask, nullptr);
}

void unblock_sig(sigset_t *mask) {
  sigprocmask(SIG_UNBLOCK, mask, nullptr);
}