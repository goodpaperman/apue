#include "apue_db.h"

DBHANDLE db_open (char const* pathname, int oflag, .../*mode*/)
{
    return 0; 
}

void db_close (DBHANDLE db)
{
}

int db_store (DBHANDLE db, const char *key, const char *data, int flag)
{
    return -1; 
}

char* db_fetch (DBHANDLE db, char *key)
{
    return 0; 
}

int db_delete (DBHANDLE db, const char *key)
{
    return -1; 
}

void db_rewind (DBHANDLE db)
{
}

char *db_nextrec (DBHANDLE db, char *key)
{
    return 0; 
}

