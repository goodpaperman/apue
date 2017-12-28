#include "../apue.h" 


int main ()
{
    pid_t ret = 0; 
    ret = fork (); 
    if (ret < 0)
        err_sys ("fork error"); 
    else if (ret == 0)
    {
        printf ("child run\n"); 
        exit (getpid ()); 
    }
    else 
        printf ("parent run\n"); 

    system ("ps"); 
    return 0; 
}
