#include "../apue.h" 
#include <time.h> 
#include <signal.h> 

#define USE_LOCALTIME_R

static void my_alarm (int signo)
{
    time_t tick = time (NULL); 
    struct tm *root = 0; 
    printf ("in signal handler\n"); 
#ifdef USE_LOCALTIME_R
    struct tm root_imp = { 0 }; 
    root = &root_imp; 
    if (localtime_r (&tick, root) != NULL)
    {
#else 
    if ((root = localtime (&tick)) != NULL)
    {
#endif 
        char tmp[50] = { 0 }; 
        strftime(tmp, sizeof(tmp)-1, "%H:%M:%S", root); 
        printf ("localtime (alrm) = %s\n", tmp); 
    }
    else 
        err_sys ("localtime (alrm) error"); 

    alarm (1); 
}

int main (void)
{
    //signal (SIGALRM, my_alarm); 
    struct sigaction act; 
    act.sa_handler = my_alarm; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction (SIGALRM, &act, NULL); 

    time_t tick = time(NULL); 
    struct tm *yunh; 
    alarm (1); 
    for (;;)
    {
#ifdef USE_LOCALTIME_R
        struct tm yunh_imp = { 0 }; 
        yunh = &yunh_imp; 
        if (localtime_r (&tick, yunh) != NULL)
        {
#else 
        if ((yunh = localtime (&tick)) != NULL)
        {
#endif 
            char tmp[50] = { 0 }; 
            strftime(tmp, sizeof(tmp)-1, "%H:%M:%S", yunh); 
            printf ("localtime (main) = %s\n", tmp); 
        }
        else 
            err_sys ("localtime (main) error"); 

        usleep (30);
        //usleep (3000); 
        //usleep (300000); 
    }

    return 0; 
}
