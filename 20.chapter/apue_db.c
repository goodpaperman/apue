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
    char *idxbuf;  // the whole index content 
    char *datbuf; 
    off_t idxoff;  // index node position in file
    size_t idxlen; // length of this index node
    off_t datoff; // corresponding data position in file for this index
    size_t datlen; // corresponding data length for this index
    off_t ptrval; // next index node position in the hash chain
    off_t ptroff;  // previous index node position (for delete. and previous of first index node in the chain is the 'chainoff', see below)
    off_t chainoff;  // root element position for this index in hash table
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

static off_t _db_readidx (DB *db, off_t offset)
{
    ssize_t i; 
    char *ptr1, *ptr2; 
    char asciiptr [PTR_SZ+1], asciilen [IDXLEN_SZ+1]; 
    struct iovec iov[2]; 

    if ((db->idxoff = lseek (db->idxfd, offset, offset == 0 ? SEEK_CUR : SEEK_SET)) == -1)
        err_dump ("_db_readidx: lseek error"); 

    iov[0].iov_base = asciiptr; 
    iov[0].iov_len = PTR_SZ; 
    iov[1].iov_base = asciilen; 
    iov[1].iov_len = IDXLEN_SZ; 
    if ((i = readv (db->idxfd, &iov[0], 2)) != PTR_SZ + IDXLEN_SZ) {
        if (i == 0 && offset == 0)
            return -1; 

        err_dump ("_db_readidx: readv error of index record"); 
    }

    asciiptr [PTR_SZ] = 0; 
    db->ptrval = atol (asciiptr); 
    asciilen [IDXLEN_SZ] = 0; 
    if ((db->idxlen = atoi (asciilen)) < IDXLEN_MIN ||
            db->idxlen > IDXLEN_MAX)
        err_dump ("_db_readidx: invalid length"); 

    if ((i = read (db->idxfd, db->idxbuf, db->idxlen)) != db->idxlen)
        err_dump ("_db_readidx: read error of index record"); 
    if (db->idxbuf[db->idxlen-1] != NEWLINE)
        err_dump ("_db_readidx: missing newline"); 
    db->idxbuf[db->idxlen-1] = 0; 

    if ((ptr1 = strchr (db->idxbuf, SEP)) == NULL)
        err_dump ("_db_readidx: missing first separator"); 

    *ptr1 ++ = 0; 
    if ((ptr2 = strchr (ptr1, SEP)) == NULL)
        err_dump ("_db_readidx: missing second separator"); 

    *ptr2 ++ = 0; 
    if (strchr (ptr2, SEP) != NULL)
        err_dump ("_db_readidx: too many separators"); 

    if ((db->datoff = atol (ptr1)) < 0)
        err_dump ("_db_readidx: starting offset < 0"); 
    if ((db->datlen = atol (ptr2)) <= 0 ||
            db->datlen > DATLEN_MAX)
        err_dump ("_db_readidx: invalid length"); 

    return db->ptrval; 
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

static char* _db_readdat (DB *db)
{
    if (lseek (db->datfd, db->datoff, SEEK_SET) == -1)
        err_dump ("_db_readdat: lseek error"); 
    if (read (db->datfd, db->datbuf, db->datlen) != db->datlen)
        err_dump ("_db_readdat: read error"); 
    if (db->datbuf [db->datlen-1] != NEWLINE)
        err_dump ("_db_readdat: missing newline"); 

    db->datbuf[db->datlen-1] = 0; 
    return db->datbuf; 
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

static void _db_writedat (DB *db, char const* data, off_t offset, int whence)
{
    struct iovec iov[2]; 
    static char newline = NEWLINE; 
    if (whence == SEEK_END)
        if (writew_lock (db->datfd, 0, SEEK_SET, 0) < 0)
            err_dump ("_db_writedat: writew_lock error"); 

    if ((db->datoff = lseek (db->datfd, offset, whence)) == -1)
        err_dump ("_db_writedat: lseek error"); 

    db->datlen = strlen (data) + 1; 
    iov[0].iov_base = (char *)data; 
    iov[0].iov_len = db->datlen - 1; 
    iov[1].iov_base = &newline; 
    iov[1].iov_len = 1; 
    if (writev (db->datfd, &iov[0], 2) != db->datlen)
        err_dump ("_db_writedat: writev error of data record"); 

    if (whence == SEEK_END)
        if (un_lock (db->datfd, 0, SEEK_SET, 0) < 0)
            err_dump ("_db_writedat: un_lock error"); 
}

static void _db_writeidx (DB *db, const char *key, off_t offset, int whence, off_t ptrval)
{
    struct iovec iov[2]; 
    char asciiptrlen [PTR_SZ + IDXLEN_SZ + 1]; 
    int len; 
    char *fmt; 

    db->ptrval = ptrval; 
    if (ptrval < 0 || ptrval > PTR_MAX)
        err_quit ("_db_writeidx: invalid ptr: %d", ptrval); 

    if (sizeof (off_t) == sizeof (long long))
        fmt = "%s%c%lld%c%d\n"; 
    else 
        fmt = "%s%c%ld%c%d\n"; 

    sprintf (db->idxbuf, fmt, key, SEP, db->datoff, SEP, db->datlen); 
    if ((len = strlen (db->idxbuf)) < IDXLEN_MIN 
            || len > IDXLEN_MAX)
        err_dump ("_db_writeidx: invalid length"); 

    sprintf (asciiptrlen, "%*ld%*d", PTR_SZ, ptrval, IDXLEN_SZ, len); 
    if (whence == SEEK_END)
        if (writew_lock (db->idxfd, ((db->nhash+1) * PTR_SZ) + 1, SEEK_SET, 0) < 0)
            err_dump ("_db_writeidx: writew_lock error"); 
    
    if ((db->idxoff = lseek (db->idxfd, offset, whence)) == -1)
        err_dump ("_db_writeidx: lseek error"); 

    iov[0].iov_base = asciiptrlen; 
    iov[0].iov_len = PTR_SZ + IDXLEN_SZ; 
    iov[1].iov_base = db->idxbuf; 
    iov[1].iov_len = len; 
    if (writev (db->idxfd, &iov[0], 2) != PTR_SZ + IDXLEN_SZ + len)
        err_dump ("_db_writeidx: writev error of index record"); 

    if (whence == SEEK_END)
        if (un_lock (db->idxfd, ((db->nhash+1) * PTR_SZ) + 1, SEEK_SET, 0) < 0)
            err_dump ("_db_writeidx: un_lock error"); 
}

static void _db_writeptr (DB *db, off_t offset, off_t ptrval)
{
    char asciiptr [PTR_SZ + 1]; 
    if (ptrval < 0 || ptrval > PTR_MAX)
        err_quit ("_db_writeptr: invalid ptr: %d", ptrval); 

    sprintf (asciiptr, "%*ld", PTR_SZ, ptrval); 
    if (lseek (db->idxfd, offset, SEEK_SET) == -1)
        err_dump ("_db_writeptr: lseek error to ptr field"); 
    if (write (db->idxfd, asciiptr, PTR_SZ) != PTR_SZ)
        err_dump ("_db_writeptr: write error of ptr field"); 
}

static void _db_dodelete (DB *db)
{
    int i; 
    char *ptr; 
    off_t freeptr, saveptr; 
    for (ptr = db->datbuf, i=0; i < db->datlen-1; ++ i)
        *ptr++ = SPACE; 

    *ptr = 0; 
    ptr = db->idxbuf; 
    while (*ptr)
        *ptr ++ = SPACE; 

    // lock free list to relase index node
    if (writew_lock (db->idxfd, FREE_OFF, SEEK_SET, 1) < 0)
        err_dump ("_db_dodelete: writew_lock error"); 

    _db_writedat (db, db->datbuf, db->datoff, SEEK_SET); 
    freeptr = _db_readptr (db, FREE_OFF); 
    saveptr = db->ptrval; 
    _db_writeidx (db, db->idxbuf, db->idxoff, SEEK_SET, freeptr); 
    _db_writeptr (db, FREE_OFF, db->idxoff); 
    _db_writeptr (db, db->ptroff, saveptr); 
    if (un_lock (db->idxfd, FREE_OFF, SEEK_SET, 1) < 0)
        err_dump ("_db_dodelete: un_lock error"); 
}

int db_delete (DBHANDLE h, const char *key)
{
    DB *db = (DB *)h; 
    int rc = 0; 
    if (_db_find_and_lock (db, key, 1) == 0) {
        _db_dodelete (db); 
        db->cnt_delok ++; 
    } else {
        rc = -1; 
        db->cnt_delerr ++; 
    }

    if (un_lock (db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
        err_dump ("db_delete: un_lock error"); 

    return -1; 
}

static int _db_findfree (DB *db, int keylen, int datlen)
{
    int rc; 
    off_t offset, nextoffset, saveoffset; 
    if (writew_lock (db->idxfd, FREE_OFF, SEEK_SET, 1) < 0)
        err_dump ("_db_findfree: writew_lock error"); 

    saveoffset = FREE_OFF; 
    offset = _db_readptr (db, saveoffset); 
    while (offset != 0) { 
        nextoffset = _db_readidx (db, offset); 
        // too strict ?
        if (strlen (db->idxbuf) == keylen
                && db->datlen == datlen)
            break; 

        saveoffset = offset; 
        offset = nextoffset; 
    }

    if (offset == 0)
        rc = -1; 
    else  {
        // remove this empty node from free list
        _db_writeptr (db, saveoffset, db->ptrval); 
        rc = 0; 
    }

    if (un_lock (db->idxfd, FREE_OFF, SEEK_SET, 1) < 0)
        err_dump ("_db_findfree: un_lock error"); 

    return rc; 
}

int db_store (DBHANDLE h, const char *key, const char *data, int flag)
{
    DB *db = (DB *)h; 
    int rc, keylen, datlen; 
    off_t ptrval; 
    if (flag != DB_INSERT && flag != DB_REPLACE && flag != DB_STORE) {
        errno = EINVAL; 
        return -1; 
    }

    keylen = strlen (key); 
    datlen = strlen (data) + 1; 
    if (datlen < DATLEN_MIN || datlen > DATLEN_MAX)
        err_dump ("db_store: invalid data length"); 

    if (_db_find_and_lock (db, key, 1) < 0) { 
        if (flag == DB_REPLACE) { 
            rc = -1; 
            db->cnt_storerr ++; 
            errno = ENOENT; 
            goto doreturn; 
        }

        ptrval = _db_readptr (db, db->chainoff); 
        if (_db_findfree (db, keylen, datlen) < 0) {
            _db_writedat (db, data, 0, SEEK_END); 
            _db_writeidx (db, key, 0, SEEK_END, ptrval); 
            _db_writeptr (db, db->chainoff, db->idxoff); 
            db->cnt_stor_ins_app ++; 
        } else {
            _db_writedat (db, data, db->datoff, SEEK_SET); 
            // insert on to chain head (ptrval)
            _db_writeidx (db, key, db->idxoff, SEEK_SET, ptrval); 
            _db_writeptr (db, db->chainoff, db->idxoff); 
            db->cnt_stor_ins_ru ++; 
        }
    } else {
        if (flag == DB_INSERT) {
            rc = 1; 
            db->cnt_storerr ++; 
            goto doreturn; 
        }

        if (datlen != db->datlen) {
            _db_dodelete (db); 
            // _db_dodelete will mass db->ptrval
            ptrval = _db_readptr (db, db->chainoff); 
            _db_writedat (db, data, 0, SEEK_END); 
            _db_writeidx (db, key, 0, SEEK_END, ptrval); 
            _db_writeptr (db, db->chainoff, db->idxoff); 
            db->cnt_stor_rep_app ++; 
        } else {
            _db_writedat (db, data, db->datoff, SEEK_SET); 
            db->cnt_stor_rep_ow ++; 
        }
    }

    rc = 0; 
doreturn:
    if (un_lock (db->idxfd, db->chainoff, SEEK_SET, 1) < 0)
        err_dump ("db_store: un_lock error"); 

    return rc; 
}

void db_rewind (DBHANDLE db)
{
}

char *db_nextrec (DBHANDLE db, char *key)
{
    return 0; 
}

