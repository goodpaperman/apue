#include "../apue.h" 
#include <pwd.h> 
#include <signal.h> 

#define USE_GETPWNAM_R

static void my_alarm (int signo)
{
    struct passwd *root = 0; 
    printf ("in signal handler\n"); 
#ifdef USE_GETPWNAM_R
    char buf[1024] = { 0 }; 
    struct passwd *ptr = 0; 
    struct passwd root_imp = { 0 }; 
    root = &root_imp; 
    if (getpwnam_r ("root", root, buf, sizeof(buf), &ptr) == 0)
    {
        root = ptr; 
#else 
    if ((root = getpwnam ("root")) != NULL)
    {
#endif 
        printf ("getpwnam (root) = %s\n", root->pw_name); 
    }
    else 
        err_sys ("getpwnam (root) error"); 

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

    struct passwd *yunh; 
    alarm (1); 
    for (;;)
    {
#ifdef USE_GETPWNAM_R
        char buf[1024] = { 0 }; 
        struct passwd *ptr = 0; 
        struct passwd yunh_imp = { 0 }; 
        yunh = &yunh_imp; 
        if (getpwnam_r ("yunh", yunh, buf, sizeof(buf), &ptr) == 0)
        {
            yunh = ptr; 
#else 
        if ((yunh = getpwnam ("yunh")) != NULL)
        {
#endif 
            if (strcmp (yunh->pw_name, "yunh") != 0)
                printf ("return value corrupted! pw_name = %s\n", yunh->pw_name); 
            else 
                printf ("getpwnam (yunh) = %s\n", yunh->pw_name); 
        }
        else 
            err_sys ("getpwnam error"); 

        //usleep (300000); 
    }

    return 0; 
}
