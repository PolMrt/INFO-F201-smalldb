#ifndef _DB_HPP
#define _DB_HPP

#include <vector>

#include "student.hpp"

/**
 * Database structure.
 **/
struct database_t {
  std::vector<student_t> data; /** Students */
  const char*            path; /** DB path */
};

/**
 * Initialize db with the content of file at path
 **/
void db_load(database_t *db, const char *path);

/**
 * Add a student to the database.
 **/
void db_add(database_t *db, student_t s);

/**
 * Delete a student from the database.
 *
 * Return the number of deleted students
 **/
size_t db_delete(database_t *db, student_t *s);

/**
 * Save the content of a database_t in db->path.
 **/
void db_save(database_t *db);

#endif  // _DB_HPP
