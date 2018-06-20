#include "../apue.h" 

void sig_eater (int signum)
{
    printf ("got signal %d\n", signum); 
    signal(signum, sig_eater); 
}


void test_1 (int signo)
{
    printf ("orginal self\n"); 
#if 0
    signal (signo, sig_eater); 
#else
    signal (signo, SIG_IGN); 
#endif 
    char tmp[128] = { 0 }; 
    sprintf (tmp, "%d", signo); 
    printf ("exec self\n"); 
    execl ("./execsig", "execsig", tmp, NULL); 
}

void test_2 (int signo)
{
    printf ("secondary self\n"); 
#if 1
    __sighandler_t old = signal (signo, sig_eater); 
    printf ("old handler %d\n", old); 
#endif
}

int main (int argc, char *argv[])
{
    if (argc > 1)
        test_2 (atoi (argv[1])); 
    else 
        test_1 (SIGINT); 

    pause (); 
}
