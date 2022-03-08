#pragma once
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

sqlite3 * p_db;

int zchat_open_db(const char * db_file_path);

int zchat_exec_sql(const char * sql,sqlite3_callback callback,void * callback_arg,char **errmsg);

int zchat_query_table(const char * sql,char ** result,int * row,int *col,char ** errmsg);

int zchat_free_result(char ** result);

void zchat_close_db();