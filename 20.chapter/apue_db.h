#ifndef __APUE_DB_H__
#define __APUE_DB_H__

typedef DBHANDLE void*

DBHANDLE db_open (char const* pathname, int oflag, .../*mode*/); 
void db_close (DBHANDLE db); 
int db_store (DBHANDLE db, const char *key, const char *data, int flag); 
char* db_fetch (DBHANDLE db, char *key); 
int db_delete (DBHANDLE db, const char *key); 
void db_rewind (DBHANDLE db); 
char *db_nextrec (DBHANDLE db, char *key); 

#endif
