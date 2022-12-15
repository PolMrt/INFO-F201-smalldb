#ifndef _GUARD_H
#define _GUARD_H

/**
 * @brief To be called before doing writing operations to the databse.
 * 
 */
void write_guard_before();

/**
 * @brief To be called after doing writing operations to the databse.
 * 
 */
void write_guard_after();

/**
 * @brief To be called before doing reading operations to the databse.
 * 
 */
void read_guard_before();

/**
 * @brief To be called after doing reading operations to the databse.
 * 
 */
void read_guard_after();

#endif  // _GUARD_H