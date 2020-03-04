#ifndef __APUE_DB_H__
#define __APUE_DB_H__

typedef void* DBHANDLE;
#define DB_INSERT 1
#define DB_REPLACE 2
#define DB_STORE 3

DBHANDLE db_open (char const* pathname, int oflag, .../*mode*/); 
void db_close (DBHANDLE db); 
int db_store (DBHANDLE db, const char *key, const char *data, int flag); 
char* db_fetch (DBHANDLE db, char *key); 
int db_delete (DBHANDLE db, const char *key); 
void db_rewind (DBHANDLE db); 
char *db_nextrec (DBHANDLE db, char *key); 

#endif
