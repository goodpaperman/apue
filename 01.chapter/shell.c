#include "../apue.h"
#include "../log.h" 
#include <sys/wait.h>
#include <errno.h> 
#include <signal.h> 
#include <linux/limits.h> 

#define MAX_BKGND 10
// SIGSTOP is non-maskable, use SIGUSR1 instead
#define SIG_STOP SIGUSR1
static void sighandler (int); 
static void sigchild (int, siginfo_t *, void*); 

#define JOB_FORE 1
#define JOB_BACK 2
#define JOB_STOP 3

char g_sig = ' '; 

struct jobinfo
{
    char cmd[PATH_MAX]; 
    pid_t pid; 
    int state; 
}; 

#define MAX_JOB 16
struct jobinfo jobs[MAX_JOB]; 

struct jobinfo* forejob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        if (jobs[i].state == JOB_FORE)
            return &jobs[i]; 

    return NULL; 
}

struct jobinfo* findjob (pid_t pid)
{
    for (int i=0; i<MAX_JOB; ++ i)
        if (jobs[i].pid == pid)
            return &jobs[i]; 

    return NULL; 
}

void setfpg (pid_t pgid)
{
    // use stdin as default tty
    //int tty = STDIN_FILENO; //open ("/dev/tty", O_RDWR); 
    //if (tty == -1)
    //{
    //    writeLog ("open default tty failed, errno %d\n", errno); 
    //    return; 
    //}
    int ret = 0; 

#ifdef  USE_TIOCSPGRP
    // if foreground process group id differs with controlling process, 
    // when it exits, process group dies, the controlling tty attached (by tcsetpgrp)
    // will be destroyed together, and controlling process will have no controlling tty !
    // so keep foreground process same group id with controlling process !!
    ret = tcsetpgrp (STDIN_FILENO, pgid); 
    if (ret != 0)
        writeLog ("%d.%d.%d.%d tcsetpgrp failed, pgid %d, errno %d, current forepg %d\n", getsid(getpid ()), getpgrp(), getppid(), getpid(), pgid, errno, tcgetpgrp (0)); 
    else 
        writeLog ("%d.%d.%d.%d tcsetpgrp %d OK, current fore process group: %d\n", getsid(getpid ()), getpgrp(), getppid(), getpid(), pgid, tcgetpgrp (0)); 
#endif 

    //close (tty); 
}

int addjob (char const* cmd, pid_t pid, int state)
{
    int n = 0; 
    if (state == JOB_FORE)
    {
        for (; n<MAX_JOB; ++ n)
            if (jobs[n].pid != -1 && jobs[n].state == JOB_FORE)
                break; 

        if (n != MAX_JOB)
        {
            // has fore job !
            writeLog ("has fore job %d, skip adding new job %d\n", jobs[n].pid, pid); 
            return 0; 
        }
    }

    for (n=0; n<MAX_JOB; ++ n)
        if (jobs[n].pid == -1)
            break; 

    if (n == MAX_JOB)
        return 0; 

    jobs[n].pid = pid; 
    strcpy (jobs[n].cmd, cmd); 
    jobs[n].state = state; 
    writeLog ("add %s job %d: [%d] %s\n", state == JOB_FORE ? "fore" : "back", pid, strlen(cmd), cmd); 
    if (state == JOB_FORE)
    {
        setfpg (getpgid(pid)); 
    }

    return 1; 
}

void deletejob (pid_t pid)
{
    for (int i=0; i<MAX_JOB; ++ i)
    {
        if (jobs[i].pid == pid)
        {
            int fore = jobs[i].state == JOB_FORE; 
            jobs[i].pid = -1; 
            jobs[i].state = 0; 
            jobs[i].cmd[0] = 0; 
            writeLog ("delete job %d\n", pid); 
            if (fore)
            {
                // reset fore process group to controlling process
                setfpg (getpgrp ()); 
            }

            break;
        }
    }
}

void displayjob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        if (jobs[i].pid != -1)
        {
            if (jobs[i].state != JOB_FORE)
                printf ("[%d] %d %s (%s)\n", i, jobs[i].pid, jobs[i].cmd, jobs[i].state == JOB_STOP ? "stop":"running"); 
            else 
                printf ("[%d] %d %s (%s)\n", i, jobs[i].pid, jobs[i].cmd, "fore"); 
        }

    //writeLog ("\n"); 
}

void initjob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        jobs[i].pid = -1; 

    writeLog ("current fore process group: %d\n", tcgetpgrp (0)); 
}

void do_wait (pid_t cid, int retry)
{
    pid_t pid = 0; 
    int status = 0, error = 0; 
    do
    {
        //writeLog ("prepare to wait child, last sig: %c\n", g_sig); 
        //g_sig = ' '; 
        pid = waitpid (cid, &status, 0); 
        if (pid < 0)
        {
            error = errno; 
            writeLog ("%s waitpid error %d, lastsig %c\n", retry ? "sync" : "async", errno, g_sig); 
        }
        else 
        {
            writeLog ("%s waitpid %d %d\n", retry ? "sync" : "async", cid, status); 
            deletejob (cid); 
        }
        // only break if child stop
    } while (retry && pid < 0 && error == EINTR && g_sig != 'Z'); 

    if (pid < 0 && error == EINTR)
        writeLog ("break wait due to sig: %c\n", g_sig); 
}

int check_child_state (struct jobinfo* job, int fore)
{
    int status = 0; 
    int ret = waitpid (job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED); 
    if (ret == job->pid)
    {
        writeLog ("waitpid status 0x%x\n", status); 
        if (WIFCONTINUED(status))
        {
            job->state = fore ? JOB_FORE : JOB_BACK; 
            if (fore)
            {
                writeLog ("set child fore and wait\n"); 
                do_wait (job->pid, 1); 
            }
            else 
                writeLog ("set child back running\n"); 
        }
        else if (WIFSTOPPED(status))
        {
            // not actually changed 
            writeLog ("child stopped by sig %d\n", WSTOPSIG(status)); 
            if (job->state == JOB_FORE)
                setfpg (getpgrp ()); 

            writeLog ("set child back\n"); 
            job->state = JOB_STOP; 
        }
        else
        {
            writeLog ("is child die ?\n"); 
        }
    }
    else if (ret == 0)
    {
        writeLog ("child state not change, nothing to do\n"); 
        //writeLog ("child state not change, using state given\n"); 
        //if (cont)
        //{
        //    job->state = fore ? JOB_FORE : JOB_BACK; 
        //    if (fore)
        //    {
        //        setfpg (getpgid(job->pid)); 
        //        writeLog ("set child fore and wait\n"); 
        //        do_wait (job->pid, 1); 
        //    }
        //    else 
        //        writeLog ("set child back running\n"); 
        //}
        //else 
        //{
        //    if (job->state == JOB_FORE)
        //        setfpg (getpgrp ()); 

        //    writeLog ("set child back\n"); 
        //    job->state = JOB_STOP; 
        //}
    }
    else 
    {
        writeLog ("waitpid failed, errno %d\n", errno); 
    }

    return ret; 
}

int do_builtin (char const* buf)
{
    int ret = 0; 
    writeLog ("%s\n", buf); 
    if (strlen(buf) == 0)
    {
        // may be Ctrl+Z ?
        return 1; 
    }
    else if (strncmp (buf, "jobs", 4) == 0)
    {
        pid_t fore = tcgetpgrp (0); 
        writeLog ("curent task is %d\n", fore); 
        displayjob (); 
        return 1; 
    }
    else if (strncmp (buf, "fg", 2) == 0 || 
            strncmp (buf, "bg", 2) == 0)
    {
        int n = atoi (buf+3); 
        if (n >= MAX_JOB || jobs[n].pid == -1)
            // no exact check!
                //|| (buf[0] == 'b' && jobs[n].state != JOB_STOP)/*must be STOP*/
                //|| (buf[0] == 'f' && jobs[n].state == JOB_FORE)/*can not be FORE*/)
        {
            writeLog ("slot empty\n"); 
            return 2; 
        }

        // before send CONT to child,
        // block SIGCHILD to avoid competition.
        sigset_t mask; 
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);

        // set child fore before kill 
        // to avoid SIGTTIN sent and child stopped again !
        if (buf[0] == 'f')
        {
            setfpg (getpgid(jobs[n].pid)); 
            writeLog ("set child fore before kill\n"); 
        }

        ret = kill(-jobs[n].pid, SIGCONT); 
        if (ret != 0)
        {
            setfpg (getpgrp ()); // restore old on failure
            writeLog ("kill CONT %d failed, errno %d\n", jobs[n].pid, errno); 
        }
        else 
        {
            writeLog ("kill CONT %d OK\n", jobs[n].pid); 

            //usleep (1000);  // to wait child change state
            check_child_state (&jobs[n], buf[0] == 'f'); 
        }

        sigprocmask(SIG_UNBLOCK, &mask, NULL); 
        return 1; 
    }

    return 0; 
}

    int 
main (void)
{
    char buf [MAXLINE]; 
    pid_t pid; 
    int ret; 
    openLog ("shell.log"); 
    writeLog ("tty: %s\n", ttyname (STDIN_FILENO)); 

#ifndef USE_TIOCSPGRP
    signal (SIGINT, sighandler); 
    //signal (SIGSTOP, sighandler);
    signal (SIGTSTP, sighandler); 
    signal (SIGQUIT, sighandler); 
    //signal (SIGHUP, sighandler); 
    //signal (SIGCHLD, sighandler); 
#else 
    signal (SIGTTOU, SIG_IGN); 
    //signal (SIGTTIN, sighandler); 
#endif 

    struct sigaction act; 
    sigemptyset(&act.sa_mask); 
    act.sa_sigaction = sigchild; 
#ifndef USE_TIOCSPGRP
    // no SIGCHLD when child stopped.
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#else 
    act.sa_flags = SA_SIGINFO;
#endif 

    sigaction (SIGCHLD, &act, 0); 

    initjob (); 
    //writeLog ("%s%% ", ttyname(0)); 
    while (1) {
        //while (fgets (buf, MAXLINE, stdin) != 0) {
        errno = 0; 
        char *ptr = fgets (buf, MAXLINE, stdin); 
        if (ptr == 0)
        {
            if (errno == EINTR) {
                //writeLog ("fgets terminal by signal, contining..\n%% "); 
                continue; 
            }
            else 
            { 
                writeLog ("fgets failed error %d\n", errno); 
                sleep (10); 
                break; 
            }
        }

        while (strlen(buf) > 0 && buf [strlen (buf) - 1] == '\n') 
            buf [strlen (buf) - 1] = 0; 

        if (do_builtin (buf))
        {
            // eat builtin command
            printf ("%% "); 
            continue; 
        }

#if 1
        int n = 0; 
        char *args[10] = { 0 }; 
        ptr = buf; 
        while ((args[n] = strtok(ptr, " ")) != NULL)
        {
            //writeLog ("split: %s\n", args[n]); 
            ptr = NULL; 
            n++; 
        }

        int backgnd = 0; 
        if (n > 1 && args[n-1][0] == '&')
        {
            backgnd = 1; 
            n--; 
        }

        args[n] = NULL; 

        // to avoid wait failed with ECHILD
        sigset_t mask; 
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);
        if ((pid = fork ()) < 0) {
            err_sys ("fork error"); 
        } else if (pid == 0) { 
            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            // every task in seperate group
            ret = setpgid (0, 0); 
            if (ret != 0)
                err_ret ("setpgid (0, 0) failed"); 

            if (!backgnd)
                setfpg (getpgrp ()); 

            execvp (args[0], args); 
            err_ret ("couldn't execute: %s", buf); 
            exit (127); 
        }

        ret = setpgid (pid, 0); 
        if (ret != 0)
            err_ret ("setpgid (%d, 0) failed", pid); 

        ret = addjob(buf, pid, backgnd ? JOB_BACK : JOB_FORE); 
        // unblock signal untill addjob over, 
        // to avoid delete job in signal handler failure.
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        if (ret && !backgnd)
        {
            do_wait (pid, 1); 
        }

        // parent
        printf ("%% "); 
#else 
        system (buf); 
#endif 
    } 

    closeLog (); 
    exit (0); 
}

void sighandler (int signo)
{
    int ret = 0; 
    writeLog ("interrupt %d\n%% ", signo); 

    //signal (SIGSTOP, sighandler); 
    //signal (SIGCONT, sighandler); 
    if (signo == SIGHUP)
    {
        g_sig = 'H'; 
        exit (SIGHUP); 
    }
    else if (signo == SIGTSTP)
    {
        g_sig = 'Z'; 
        struct jobinfo* job = forejob (); 
        if (job == NULL)
            writeLog ("no active foreground task running!\n"); 
        else 
        {
            ret = kill (-job->pid, SIGSTOP); 
            if (ret != 0)
                writeLog ("kill TSTP %d failed, errno %d\n", job->pid, errno); 
            else 
            {
                writeLog ("kill TSTP %d OK\n", job->pid); 
                job->state = JOB_STOP; 
            }
        }
    }
    else if (signo == SIGINT || 
            signo == SIGQUIT)
    {
        g_sig = signo == SIGINT ? 'I' : 'Q'; 
        struct jobinfo* job = forejob (); 
        if (job == NULL)
            writeLog ("no active foreground task running!\n"); 
        else 
        {
            ret = kill (-job->pid, signo); 
            writeLog ("kill %d %d return %d, errno %d\n", signo, job->pid, ret, errno); 
        }
    }
    else
    {
        writeLog ("recv signal %d\n", signo); 
    }

    signal (signo, sighandler); 
}

void sigchild (int signo, siginfo_t *info, void* param)
{
    if (signo == SIGCHLD)
    {
        if (info->si_code == CLD_STOPPED)
        {
            g_sig = 'Z'; 
            // not die, just stopped, change it backgrounding.
            // note here 2 scenes, 
            // first child is foreground process and receive Ctrl+Z; 
            // second child is background process and want read/write something
            //     to console and receive SIGTTIN/SIGTTOU and stopped.
            //
            writeLog ("child %d stop\n", info->si_pid); 
            struct jobinfo* job = findjob (info->si_pid); 
            if (job == NULL)
            {
                writeLog ("no task find!\n"); 
            }
            else
            {
                check_child_state (job, 0); 
                //switch (job->state)
                //{
                //    case JOB_FORE:
                //        // reset fore process group
                //        job->state = JOB_STOP; 
                //        setfpg (getpgrp ()); 
                //        break; 
                //    case JOB_BACK:
                //        // just update state is OK
                //        job->state = JOB_STOP; 
                //        writeLog ("update task %d state to stop\n", job->pid); 
                //        break; 
                //    default:
                //        break; 
                //}
            }
        }
        else if (info->si_code == CLD_CONTINUED)
        {
            g_sig = 'C'; 
            writeLog ("child %d continue\n", info->si_pid); 
            struct jobinfo* job = findjob (info->si_pid); 
            if (job == NULL)
                writeLog ("no task find!\n"); 
            else 
            {
                check_child_state (job, 0); 
                //if (job->state == JOB_STOP)
                //{
                //    // just update state is OK.
                //    job->state = JOB_BACK;  // NOT JOB_FORE !
                //    writeLog ("update task %d state to running\n", job->pid); 
                //}
            }
        }
        else if (info->si_code == CLD_EXITED ||
                info->si_code == CLD_KILLED || 
                info->si_code == CLD_DUMPED)
        {
            g_sig = 'D'; 
            writeLog ("child %d die\n", info->si_pid); 
            do_wait (info->si_pid, 0); 
        }
        else 
        {
            g_sig = 'O'; 
            writeLog ("child %d state changed: %d\n", info->si_pid, info->si_code); 
        }
    }
}
