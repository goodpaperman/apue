/* 
 * Usage: getopt [-i] [-u username] [-z] filename
 *
 */

#include "../apue.h"

int log_to_stderr = 1; 

int main (int argc, char *argv[])
{
    int c, i; 
#if 0
    printf ("old opterr = %d\n", opterr); 
    opterr = 0; 
#endif

    char fmt[1024] = { 0 }; 
    char *abc = "abcdefghijklmnopqrtsuvwxyz"; 
    char *ABC = "A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:R:S:T:U:V:W:X:Y:Z:"; 
    strcat (fmt, abc); 
    strcat (fmt, ABC); 
    while ((c = getopt (argc, argv, fmt)) != -1) {
        printf ("got option [%d]: '%c' ('%c')", optind, c, optopt); 
        if (optarg)
            printf (" arg: '%s'", optarg); 

        printf ("\n"); 
    }

    printf ("end up at %d\n", optind); 
    for (i = 0; i<argc; ++ i)
        printf ("%s  ", argv[i]); 

    printf ("\n"); 
    if (optind < argc)
        printf ("some argument left, from %s\n", argv[optind]); 
    exit (1); 
}
