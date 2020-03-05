#include "../apue.h"
#include "apue_db.h"
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>


// for index & date file
#define IDXLEN_SZ 4 // index record length (number lies before key)
#define SEP ':' // separator char in index record
#define SPACE ' ' // space character
#define NEWLINE '\n' // newline character

// for index file only (hash & free list)
#define PTR_SZ 6 // size of ptr field in hash chain
#define PTR_MAX 999999 // max file offset = 10 ** PTR_SZ - 1
#define NHASH_DEF 137 // default hash table size
#define FREE_OFF 0 // free list offset in index file
#define HASH_OFF PTR_SZ // hash table offset in index file

typedef unsigned long DBHASH;  // hash value
typedef unsigned long COUNT;  // unsigned counter

typedef struct {
    // global 
    int idxfd; 
    int datfd; 
    char *name; 
    DBHASH nhash;  // NHASH_DEF

    // for current index & date
    char *idxbuf; 
    char *datbuf; 
    off_t idxoff;  
    size_t idxlen; 
    off_t datoff; 
    size_t datlen; 
    off_t ptrval; //
    off_t ptroff;  // 
    off_t chainoff;  // field 1
    off_t hashoff;  // HASH_OFF

    // for staticstic
    COUNT cnt_delok; 
    COUNT cnt_delerr; 
    COUNT cnt_fetchok; 
    COUNT cnt_fetcherr; 
    COUNT cnt_nextrec; 
    COUNT cnt_stor_ins_app; 
    COUNT cnt_stor_ins_ru; 
    COUNT cnt_stor_rep_app; 
    COUNT cnt_stor_rep_ow; 
    COUNT cnt_storerr; 
} DB; 

static DB* _db_alloc (int); 
static void _db_dodelete (DB *); 
static int _db_find_and_lock (DB*, const char *, int); 
static int _db_findfree (DB*, int, int); 
static void _db_free (DB*); 
static DBHASH _db_hash (DB*, const char *); 
static char* _db_readdat (DB *); 
static off_t _db_readidx (DB *, off_t); 
static off_t _db_readptr (DB *, off_t); 
static void _db_writedat (DB *, const char *, off_t, int); 
static void _db_writeidx (DB *, const char *, off_t, int, off_t); 
static void _db_writeptr (DB *, off_t, off_t); 


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

