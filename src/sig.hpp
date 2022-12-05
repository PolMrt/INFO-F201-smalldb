#ifndef _SIGNAL_CUSTOM_HPP
#define _SIGNAL_CUSTOM_HPP

#include <signal.h>

void block_sig(sigset_t *mask, int signal);

void unblock_sig(sigset_t *mask);

#endif
