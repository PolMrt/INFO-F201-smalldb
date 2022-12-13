#ifndef _QUERIES_HPP
#define _QUERIES_HPP

#include <cstdio>
#include <vector>
#include <string>

#include "db.hpp"

enum QUERY_STATUS { QUERY_SUCCESS, QUERY_BAD_TYPE, QUERY_BAD_FORMAT, QUERY_TOO_LONG, QUERY_BAD_FILTER, QUERY_BAD_UPDATE, QUERY_ID_EXISTS_INSERT };
enum QUERY_TYPE { QUERY_SELECT, QUERY_UPDATE, QUERY_DELETE, QUERY_INSERT };
struct query_result_t {
  std::vector<student_t> students; /** A list of students **/
  char* query;        /** The actual query that was submitted **/
  QUERY_TYPE type;
  QUERY_STATUS status = QUERY_SUCCESS; /** The return status of the query **/
  char error_message[1024];
};

std::string get_query_type(const query_result_t& query_result);

// execute_* //////////////////////////////////////////////////////////////////

void execute_select(query_result_t& query_result, database_t* const db, const char* const field,
                    const char* const value);

void execute_update(query_result_t& query_result, database_t* const db, const char* const ffield,
                    const char* const fvalue, const char* const efield, const char* const evalue);

void execute_insert(query_result_t& query_result, database_t* const db, const char* const fname,
                    const char* const lname, const unsigned id, const char* const section,
                    const tm birthdate);

void execute_dump(query_result_t& query_result, database_t* const db);

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select(query_result_t& query_result, database_t* db, const char* const query);

void parse_and_execute_update(query_result_t& query_result, database_t* db, const char* const query);

void parse_and_execute_insert(query_result_t& query_result, database_t* db, const char* const query);

void parse_and_execute_delete(query_result_t& query_result, database_t* db, const char* const query);

void parse_and_execute(query_result_t& query_result, database_t* db, const char* const query);

// query_fail_* ///////////////////////////////////////////////////////////////

/** Those methods write a descriptive error message on fout */

void query_fail_bad_query_type(query_result_t& query_result);

void query_fail_bad_format(query_result_t& query_result, const char* const query_type);

void query_fail_too_long(query_result_t& query_result, const char* const query_type);

void query_fail_bad_filter(query_result_t& query_result, const char* const field, const char* const filter);

void query_fail_bad_update(query_result_t& query_result, const char* const field, const char* const filter);

void query_fail_id_exists(query_result_t& query_result, const char* const field, const char* const filter);

#endif
