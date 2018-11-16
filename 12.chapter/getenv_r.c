#include "../apue.h" 
#include <linux/limits.h> 
#include <pthread.h> 

static char envbuf[ARG_MAX]; 
extern char **environ; 

char* my_getenv (char const* name)
{
    int i, len; 
    len = strlen (name); 
    for (i=0; environ[i] != NULL; i++) { 
        if ((strncmp (name, environ[i], len) == 0)
            && (environ[i][len] == '=')) { 
            strcpy (envbuf, &environ[i][len+1]); 
            return envbuf; 
        }
    }

    return NULL; 
}

int main (int argc, char *argv[])
{
    char *key = "PATH"; 
    if (argc > 1)
       key = argv[1]; 

    char const* envstr = my_getenv (key); 
    if (envstr)
        printf ("%s: %s\n", key, envstr); 
    else 
        printf ("%s not found\n", key); 

    return 0; 
}

