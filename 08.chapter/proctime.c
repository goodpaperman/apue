#include "../apue.h" 
#include <sys/times.h> 


static void pr_times (clock_t, struct tms *, struct tms *); 
static void do_cmd (char *); 

int main (int argc, char *argv[])
{
    int i; 
    for (i=1; i<argc; i++)
        do_cmd (argv[i]); 

    exit (0); 
}

static void do_cmd (char *cmd)
{
    struct tms start, stop; 
    clock_t begin, end; 
    int status; 

    fprintf (stderr, "\ncommand: %s\n", cmd); 
    if ((begin = times (&start)) == -1)
        err_sys ("times error"); 
    if ((status = system (cmd)) < 0)
        err_sys ("system () error"); 
    if ((end = times (&stop)) == -1)
        err_sys ("times error"); 

    pr_times (end-begin, &start, &stop); 
    pr_exit (status); 
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
