#include "../apue.h" 
#include <sys/types.h> 
#include <sys/times.h> 
#include <sys/wait.h> 


static void pr_times (clock_t, struct tms *, struct tms *); 
static void do_cmd (char *); 

int main (int argc, char *argv[])
{
    int i; 
    int status; 
    pid_t pid; 
    struct tms start, stop; 
    clock_t begin, end; 
    for (i=1; i<argc; i++)
        do_cmd (argv[i]); 

    if ((begin = times (&start)) == -1)
        err_sys ("times error"); 

    while (1)
    {
        pid = wait (&status); 
        if (pid < 0)
        {
            printf ("wait all children\n"); 
            break; 
        }

        printf ("wait child %d\n", pid); 
        pr_exit (status); 
        if ((end = times (&stop)) == -1)
            err_sys ("times error"); 

        pr_times (end-begin, &start, &stop); 
#if 0
        begin = end; 
        start = stop; 
#endif 
        printf ("---------------------------------\n"); 
    }

    exit (0); 
}

static void do_cmd (char *cmd)
{
    int status; 
    pid_t pid = fork (); 
    if (pid < 0)
        err_sys ("fork error"); 
    else if (pid == 0)
    {
        // children
        fprintf (stderr, "\ncommand: %s\n", cmd); 
        if ((status = system (cmd)) < 0)
            err_sys ("system () error"); 

        pr_exit (status); 
        exit (status); 
    }
    else 
        printf ("fork child %d\n", pid); 
}

static void pr_times (clock_t real, struct tms *start, struct tms *stop)
{
    static long clktck = 0; 
    if (clktck == 0)
        if ((clktck = sysconf (_SC_CLK_TCK)) < 0)
            err_sys ("sysconf error"); 

    clock_t diff = 0; 
    fprintf (stderr, "  real: %7.2f\n", real / (double)clktck); 
    diff = (stop->tms_utime - start->tms_utime);
    fprintf (stderr, "  user: %7.2f (%ld)\n", diff/(double) clktck, diff);  
    diff = (stop->tms_stime - start->tms_stime);
    fprintf (stderr, "  sys: %7.2f (%ld)\n", diff/(double)clktck, diff); 
    diff = (stop->tms_cutime - start->tms_cutime);
    fprintf (stderr, "  child user: %7.2f (%ld)\n", diff/(double)clktck, diff); 
    diff = (stop->tms_cstime - start->tms_cstime);
    fprintf (stderr, "  child sys: %7.2f (%ld)\n", diff/(double)clktck, diff); 
}
