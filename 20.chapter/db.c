#include "../apue.h"
#include "apue_db.h"
#include <fcntl.h>
#include <strings.h>

void Usage ()
{
    printf ("Usage: db filename [insert|delete|query] key [data]\n"); 
    exit (-1); 
}

int main (int argc, char *argv[])
{
    if (argc < 4) 
        Usage (); 

    char *filename = argv[1]; 
    char *action = argv[2]; 
    char *key = argv[3]; 
    char *data = NULL; 
    if (strcasecmp (action, "insert") == 0)
    {
        if (argc < 5)
            Usage (); 

        data = argv[4]; 
    }
    else if (strcasecmp (action, "delete") != 0 
            && strcasecmp (action, "query") != 0)
    {
        Usage (); 
    }

    DBHANDLE db; 
    if ((db = db_open (filename, O_RDWR | O_CREAT /*| O_TRUNC*/, FILE_MODE)) == NULL)
        err_sys ("db_open error"); 

    int ret = 0; 
    char *ptr = NULL; 
    if (strcasecmp (action, "insert") == 0)
        ret = db_store (db, key, data, DB_STORE); 
    else if (strcasecmp (action, "delete") == 0)
        ret = db_delete (db, key); 
    else if (strcasecmp (action, "query") == 0)
    {
        ptr = db_fetch (db, key); 
        printf ("%s === %s\n", key, ptr); 
        ptr = (char *)1; 
    }

    if (ptr == NULL)
        printf ("db %s '%s' code %d\n", action, key, ret); 

    db_close (db); 
    return 0; 
}
