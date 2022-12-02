#include "queries.hpp"

#include <iostream>

// #include "io.hpp"

// execute_* ///////////////////////////////////////////////////////////////////

void execute_select(query_result_t* query_result, database_t* const db, const char* const field,
                    const char* const value) {

  std::cout << "In select" << std::endl;
  std::function<bool(const student_t&)> predicate = get_filter(field, value);
  if (!predicate) {
    query_fail_bad_filter(query_result, field, value);
    return;
  }
  for (const student_t& s : db->data) {
    if (predicate(s)) {
      query_result->students.push_back(s);
    }
  }
}

void execute_update(query_result_t* query_result, database_t* const db, const char* const ffield, const char* const fvalue, const char* const efield, const char* const evalue) {
  std::function<bool(const student_t&)> predicate = get_filter(ffield, fvalue);
  if (!predicate) {
    query_fail_bad_filter(query_result, ffield, fvalue);
    return;
  }
  std::function<void(student_t&)> updater = get_updater(efield, evalue);
  if (!updater) {
    query_fail_bad_update(query_result, efield, evalue);
    return;
  }
  for (student_t& s : db->data) {
    if (predicate(s)) {
      updater(s);
    }
  }
}

void execute_insert(query_result_t* query_result, database_t* const db, const char* const fname,
                    const char* const lname, const unsigned id, const char* const section,
                    const tm birthdate) {
  db->data.emplace_back();
  student_t *s = &db->data.back();
  s->id = id;
  snprintf(s->fname, sizeof(s->fname), "%s", fname);
  snprintf(s->lname, sizeof(s->lname), "%s", lname);
  snprintf(s->section, sizeof(s->section), "%s", section);
  s->birthdate = birthdate;
}

void execute_delete(query_result_t* query_result, database_t* const db, const char* const field,
                    const char* const value) {
  std::function<bool(const student_t&)> predicate = get_filter(field, value);
  if (!predicate) {
    query_fail_bad_filter(query_result, field, value);
    return;
  }
  auto new_end = remove_if(db->data.begin(), db->data.end(), predicate);
  db->data.erase(new_end, db->data.end());
}

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select(query_result_t* query_result, database_t* db, const char* const query) {
  char ffield[32], fvalue[64];  // filter data
  int  counter;
  if (sscanf(query, "select %31[^=]=%63s%n", ffield, fvalue, &counter) != 2) {
    query_fail_bad_format(query_result, "select");
  } else if (static_cast<unsigned>(counter) < strlen(query)) {
    query_fail_too_long(query_result, "select");
  } else {
    execute_select(query_result, db, ffield, fvalue);
  }
}

void parse_and_execute_update(query_result_t* query_result, database_t* db, const char* const query) {
  char ffield[32], fvalue[64];  // filter data
  char efield[32], evalue[64];  // edit data
  int counter;
  if (sscanf(query, "update %31[^=]=%63s set %31[^=]=%63s%n", ffield, fvalue, efield, evalue,
             &counter) != 4) {
    query_fail_bad_format(query_result, "update");
  } else if (static_cast<unsigned>(counter) < strlen(query)) {
    query_fail_too_long(query_result, "update");
  } else {
    execute_update(query_result, db, ffield, fvalue, efield, evalue);
  }
}

void parse_and_execute_insert(query_result_t* query_result, database_t* db, const char* const query) {
  char      fname[64], lname[64], section[64], date[11];
  unsigned  id;
  tm        birthdate;
  int       counter;
  if (sscanf(query, "insert %63s %63s %u %63s %10s%n", fname, lname, &id, section, date, &counter) != 5 || strptime(date, "%d/%m/%Y", &birthdate) == NULL) {
    query_fail_bad_format(query_result, "insert");
  } else if (static_cast<unsigned>(counter) < strlen(query)) {
    query_fail_too_long(query_result, "insert");
  } else {
    execute_insert(query_result, db, fname, lname, id, section, birthdate);
  }
}

void parse_and_execute_delete(query_result_t* query_result, database_t* db, const char* const query) {
  char ffield[32], fvalue[64]; // filter data
  int counter;
  if (sscanf(query, "delete %31[^=]=%63s%n", ffield, fvalue, &counter) != 2) {
    query_fail_bad_format(query_result, "delete");
  } else if (static_cast<unsigned>(counter) < strlen(query)) {
    query_fail_too_long(query_result, "delete");
  } else {
    execute_delete(query_result, db, ffield, fvalue);
  }
}

void parse_and_execute(query_result_t* query_result, database_t* db, const char* const query) {
  if (strncmp("select", query, sizeof("select")-1) == 0) {
    parse_and_execute_select(query_result, db, query);
  } else if (strncmp("update", query, sizeof("update")-1) == 0) {
    parse_and_execute_update(query_result, db, query);
  } else if (strncmp("insert", query, sizeof("insert")-1) == 0) {
    parse_and_execute_insert(query_result, db, query);
  } else if (strncmp("delete", query, sizeof("delete")-1) == 0) {
    parse_and_execute_delete(query_result, db, query);
  } else {
    query_fail_bad_query_type(query_result);
  }
}

// query_fail_* ///////////////////////////////////////////////////////////////

void query_fail_bad_query_type(query_result_t* query_result) {
  query_result->status = QUERY_BAD_TYPE;
}

void query_fail_bad_format(query_result_t* query_result, const char * const query_type) {
  query_result->status = QUERY_BAD_FORMAT;
}

void query_fail_too_long(query_result_t* query_result, const char * const query_type) {
  query_result->status = QUERY_TOO_LONG;
}

void query_fail_bad_filter(query_result_t* query_result, const char* const field, const char* const filter) {
    query_result->status = QUERY_BAD_FILTER;
}

void query_fail_bad_update(query_result_t* query_result, const char* const field, const char* const filter) {
    query_result->status = QUERY_BAD_UPDATE;
}

