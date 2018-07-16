#include "../apue.h" 

void sigalrm (int signo)
{
    printf ("SIGALRM caught\n"); 
    signal (SIGALRM, sigalrm); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
    signal (SIGALRM, sigalrm); 
    ret = alarm (5); 
    printf ("alarm(5) return %d\n", ret); 
    sleep (2); 
    ret = alarm (5); 
    printf ("alarm(5) after 2 return %d\n", ret); 
    sleep (3); 
    ret = alarm (0); 
    printf ("alarm(0) after 3 return %d\n", ret); 
    sleep (5); 
    ret = alarm (1); 
    printf ("alarm(1) after 5 return %d\n", ret); 
    sleep (5); 
    printf ("program exit\n"); 
    return 0; 
}
