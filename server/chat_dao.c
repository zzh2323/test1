#include "chat_dao.h"


int zchat_open_db(const char * db_file_path)
{
    int ret = sqlite3_open(db_file_path,&p_db);
    if(ret != SQLITE_OK)
    {
        printf("open sqlite : %s fault!\n",db_file_path);
        return -1;
    }
    return ret;

}

int zchat_exec_sql(const char * sql,sqlite3_callback callback,void * callback_arg,char **errmsg)
{
    char *msg;
    int ret = sqlite3_exec(p_db,sql,callback,callback_arg,&msg);
    return ret;
}

int zchat_query_table(const char * sql,char ** result,int * row,int *col,char ** errmsg)
{
    char *msg;
    int ret = sqlite3_get_table(p_db,sql,&result,row,col,&msg);
    return ret;
}

int zchat_free_result(char ** result)
{
    sqlite3_free_table(result);
}

void zchat_close_db()
{
    sqlite3_close(p_db);
}