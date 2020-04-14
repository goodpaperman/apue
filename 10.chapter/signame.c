#include "../apue.h" 
#include <signal.h>
#include <string.h> 

//extern char *sys_siglist[]; 

void test_siglist ()
{
    printf ("-----sys_siglist-----\n"); 
    for (int i=0; i<_NSIG; ++ i)
        printf ("%s: %d\n", sys_siglist[i], i); 

    printf ("\n"); 
}

void test_psig ()
{
    fprintf (stderr, "-----psignal-----\n"); 
    char msg[64] = { 0 }; 
    for (int i=0; i<_NSIG; ++ i)
    {
        sprintf (msg, "%d ", i); 
        psignal (i, msg); 
        //printf ("%s", msg); 
    }

    printf ("\n"); 
}

void test_strsig ()
{
    printf ("-----strsignal-----\n"); 
    for (int i=0; i<_NSIG; ++ i)
    {
        printf ("[%d] %s\n", i, strsignal (i)); 
    }

    printf ("\n"); 
}

#if defined(SOLARIS) || defined(APUE_SIG2STR)
void test_conv ()
{
    printf ("-----str2sig-----\n"); 
    int signo; 
    char str[SIG2STR_MAX] = { 0 }; 
    for (int i=0; i<_NSIG; ++ i)
    {
        sig2str (i, str); 
        str2sig (str, &signo); 
        printf ("[%d][%d] %s\n", i, signo, str); 
    }
}
#endif

int main ()
{
    test_siglist (); 
    test_psig (); 
    test_strsig (); 
#if defined(SOLARIS) || defined(APUE_SIG2STR)
    test_conv (); 
#endif
    return 0; 
}
