#ifndef _SIGNAL_CUSTOM_HPP
#define _SIGNAL_CUSTOM_HPP

#include <signal.h>

/**
 * @brief This block the signal when creating a thread
 * 
 * @param mask the sigset (will be reused when unblocking)
 * @param signal the signal that is being maked
 */
void block_sig(sigset_t *mask, int signal);

/**
 * @brief Unblock the signal
 * 
 * @param mask  the sigset used when blocking
 */
void unblock_sig(sigset_t *mask);

#endif
