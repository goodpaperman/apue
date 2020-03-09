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
    char *name;  // dat file path
    DBHASH nhash;  // NHASH_DEF: hash size

    // for current index & date
    char *idxbuf;  // key in current index (for searching in a single hash chain)
    char *datbuf; 
    off_t idxoff;  
    size_t idxlen; 
    off_t datoff; // data offset in current index
    size_t datlen; // data length in current index
    off_t ptrval; //
    off_t ptroff;  // previous index node position (for delete)
    off_t chainoff;  // root element position for this key in hash table
    off_t hashoff;  // HASH_OFF: hash table start postion in index file

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

//static DB* _db_alloc (int); 
//static void _db_dodelete (DB *); 
//static int _db_find_and_lock (DB*, const char *, int); 
//static int _db_findfree (DB*, int, int); 
//static void _db_free (DB*); 
//static DBHASH _db_hash (DB*, const char *); 
//static char* _db_readdat (DB *); 
//static off_t _db_readidx (DB *, off_t); 
//static off_t _db_readptr (DB *, off_t); 
//static void _db_writedat (DB *, const char *, off_t, int); 
//static void _db_writeidx (DB *, const char *, off_t, int, off_t); 
//static void _db_writeptr (DB *, off_t, off_t); 

static DB* _db_alloc (int namelen)
{
    DB *db = NULL; 
    if ((db = calloc (1, sizeof (DB))) == NULL)
        err_dump ("_db_alloc: calloc error for DB"); 

    db->idxfd = db->datfd = -1; 
    // +5 to contain .dat
    if ((db->name = malloc (namelen + 5)) == NULL)
        err_dump ("_db_alloc: malloc error for name"); 

    if ((db->idxbuf = malloc (IDXLEN_MAX + 2)) == NULL)
        err_dump ("_db_alloc: malloc error for index buffer"); 
    if ((db->datbuf = malloc (DATLEN_MAX + 2)) == NULL)
        err_dump ("_db_alloc: malloc error for data buffer"); 

    return db; 
}

static void _db_free (DB *db)
{
    if (db->idxfd >= 0)
        close (db->idxfd); 
    if (db->datfd >= 0)
        close (db->datfd); 
    if (db->idxbuf != NULL)
        free (db->idxbuf); 
    if (db->datbuf != NULL)
        free (db->datbuf); 
    if (db->name != NULL)
        free (db->name); 
    free (db); 
}

DBHANDLE db_open (char const* pathname, int oflag, .../*mode*/)
{
    DB *db = NULL; 
    int len, mode; 
    size_t i; 
    char asciiptr[PTR_SZ+1]; 
    // +1 for free list (we can do this because HASH_OFF == PTR_SZ)
    // +2 for newline & null
    char hash[(NHASH_DEF + 1) * PTR_SZ + 2];
    struct stat statbuf; 
    len = strlen (pathname); 
    if ((db = _db_alloc (len)) == NULL)
        err_dump ("db_open: _db_alloc error for DB"); 
    
    db->nhash = NHASH_DEF; 
    db->hashoff = HASH_OFF; 
    strcpy (db->name, pathname); 
    strcat (db->name, ".idx"); 

    if (oflag & O_CREAT) { 
        va_list ap; 
        va_start (ap, oflag); 
        mode = va_arg (ap, int); 
        va_end (ap); 

        db->idxfd = open (db->name, oflag, mode); 
        strcpy (db->name + len, ".dat"); 
        db->datfd = open (db->name, oflag, mode); 
    } else { 
        db->idxfd = open (db->name, oflag); 
        strcpy (db->name + len, ".dat"); 
        db->datfd = open (db->name, oflag); 
    }

    if (db->idxfd < 0 || db->datfd < 0) {
        _db_free (db); 
        return NULL; 
    }

    if ((oflag & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC)) {
        if (writew_lock (db->idxfd, 0, SEEK_SET, 0) < 0)
            err_dump ("db_open: writew_lock error"); 

        if (fstat (db->idxfd, &statbuf) < 0)
            err_sys ("db_open: fstat error"); 

        if (statbuf.st_size == 0) {
            sprintf (asciiptr, "%*d", PTR_SZ, 0); 
            hash[0] = 0;  // init string not free list, see +1 below
            for (i=0; i<NHASH_DEF+1; ++i)
                strcat (hash, asciiptr); 

            strcat (hash, "\n"); 
            i = strlen (hash); 
            if (write (db->idxfd, hash, i) != i)
                err_dump ("db_open: index file init write error"); 
        }

        if (un_lock (db->idxfd, 0, SEEK_SET, 0) < 0)
            err_dump ("db_open: un_lock error"); 
    }

    // rewind to begin to prepare for read
    db_rewind (db); 
    return db; 
}

void db_close (DBHANDLE db)
{
    _db_free ((DB *)db); 
}

static DBHASH _db_hash (DB *db, const char *key)
{
    DBHASH hval = 0; 
    char c; 
    int i; 
    for (i=1; (c = *key++) != 0; i ++)
        hval += c * i; 

    return hval % db->nhash; 
}

static off_t _db_readptr (DB *db, off_t offset)
{
    char asciiptr [PTR_SZ + 1]; 
    if (lseek (db->idxfd, offset, SEEK_SET) == -1)
        err_dump ("_db_readptr: lseek error to ptr field"); 
    if (read (db->idxfd, asciiptr, PTR_SZ) != PTR_SZ)
        err_dump ("_db_readptr: read error of ptr field"); 

    asciiptr [PTR_SZ] = 0; 
    return atoi (asciiptr); 
}

static int _db_find_and_lock (DB *db, const char *key, int writelock)
{
    off_t offset, nextoffset; 
    db->chainoff = (_db_hash (db, key) * PTR_SZ) + db->hashoff; 
    db->ptroff = db->chainoff; 

    if (writelock) {
        if (writew_lock (db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
            err_dump ("_db_find_and_lock: writew_lock error"); 
    } else { 
        if (readw_lock (db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
            err_dump ("_db_find_and_lock: readw_lock error"); 
    }

    offset = _db_readptr (db, db->ptroff); 
    while (offset != 0) {
        nextoffset = _db_readidx (db, offset); 
        if (strcmp (db->idxbuf, key) == 0)
            break; 

        db->ptroff = offset; 
        offset = nextoffset; 
    }

    return offset == 0 ? -1 : 0; 
}

char* db_fetch (DBHANDLE h, char *key)
{
    DB *db =  (DB*)h; 
    char *ptr; 
    if (_db_find_and_lock (db, key, 0) < 0) { 
        ptr = NULL; 
        db->cnt_fetcherr ++; 
    } else { 
        ptr = _db_readdat (db); 
        db->cnt_fetchok ++; 
    }

    if (un_lock (db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
        err_dump ("db_fetch: un_lock error"); 

    return ptr; 
}

int db_store (DBHANDLE db, const char *key, const char *data, int flag)
{
    return -1; 
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

