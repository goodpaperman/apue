#include "../apue.h"
#include "../pty_fun.h"
#include "../tty.h"
#include <termios.h>
#include <errno.h>
#include <syslog.h>

#define BUFFSIZE 512

#ifdef __linux__
#define OPTSTR "+d:einv"
#else
#define OPTSTR "d:einv"
#endif

static volatile sig_atomic_t sigcaught; 

static void sig_term (int signo)
{
    sigcaught = 1; 
}

void loop (int ptym, int ignoreeof, int verbose)
{
    pid_t child; 
    int nread; 
    char buf[BUFFSIZE]; 

    if ((child = fork ()) < 0)
        err_sys ("fork error"); 
    else if (child == 0)
    {
        // child
        for (;;) 
        {
            if ((nread = read (STDIN_FILENO, buf, BUFFSIZE)) < 0)
                err_sys ("read error from stdin"); 
            else if (nread == 0)
            {
                if (verbose)
                    syslog (LOG_INFO, "read stdin end\n"); 

                break; 
            }
            else if (verbose)
                syslog (LOG_INFO, "read %d from stdin\n", nread); 

            if (writen (ptym, buf, nread) != nread)
                err_sys ("writen error to master pty"); 
            else if (verbose)
                syslog (LOG_INFO, "write pty master %d ok\n", nread); 
        }

        if (ignoreeof == 0)
        {
            if (verbose)
                syslog (LOG_INFO, "send SIGTERM to parent to notify our exit\n"); 

            kill (getppid (), SIGTERM); 
        }

        exit (0); 
    }

    if (verbose)
        syslog (LOG_INFO, "%u fork %u ok\n", getpid (), child); 

    // parent
    if (signal(SIGTERM, sig_term) == SIG_ERR)
        err_sys ("signal error for SIGTERM"); 
    else if (verbose)
        syslog (LOG_INFO, "setup SIGTERM handler\n"); 

    for (;;) 
    {
        if ((nread = read (ptym, buf, BUFFSIZE)) <= 0)
        {
            if (verbose)
                syslog (LOG_INFO, "read pty master failed, errno %d\n", errno); 

            break; 
        }

        if (writen (STDOUT_FILENO, buf, nread) != nread)
            err_sys ("writen error to stdout"); 
        else if (verbose)
            syslog (LOG_INFO, "write stdout %d\n", nread); 
    }

    if (sigcaught == 0)
    {
        if (verbose)
            syslog (LOG_INFO, "no SIGTERM sent from child, try kill it\n"); 

        kill (child, SIGTERM); 
    }
    else if (verbose)
        syslog (LOG_INFO, "has caught SIGTERM, exit\n"); 
}

static void set_noecho (int fd)
{
    struct termios term; 
    if (tcgetattr (fd, &term) < 0)
        err_sys ("tcgetattr error"); 

    term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); 
    term.c_oflag &= ~(ONLCR); 

    if (tcsetattr (fd, TCSANOW, &term) < 0)
        err_sys ("tcsetattr error"); 
}

int main (int argc, char *argv[])
{
    int fdm, c, ignoreeof, interactive, noecho, verbose; 
    pid_t pid; 
    char *driver; 
    char slave_name[20]; 
    struct termios orig_termios; 
    struct winsize size; 

    interactive = isatty (STDIN_FILENO); 
    ignoreeof = 0; 
    noecho = 0; 
    verbose = 0; 
    driver = NULL; 

    opterr = 0; 
    while ((c = getopt (argc, argv, OPTSTR)) != EOF)
    {
        switch (c)
        {
            case 'd':
                driver = optarg; 
                break; 
            case 'e':
                noecho = 1; 
                break; 
            case 'i':
                ignoreeof = 1; 
                break; 
            case 'n':
                interactive = 0; 
                break; 
            case 'v':
                verbose = 1; 
                break; 
            case '?':
                err_quit ("unrecongnized option: -%c", optopt); 
                break; 
        }
    }

    if (optind >= argc)
        // no program and arg following ?
        err_quit ("usage: pty [ -d driver -einv] program [ arg ... ]"); 

    if (interactive) 
    {
        if (tcgetattr (STDIN_FILENO, &orig_termios) < 0)
            err_sys ("tcgetattr error on stdin"); 
        else if (verbose)
            syslog (LOG_INFO, "tcgetattr for current tty ok\n"); 

        if (ioctl (STDIN_FILENO, TIOCGWINSZ, (char *) &size) < 0)
            err_sys ("TIOCGWINSZ error"); 
        else if (verbose)
            syslog (LOG_INFO, "TIOCGWINSZ for currrent tty ok, %d, %d\n", size.ws_row, size.ws_col); 

        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), &orig_termios, &size, verbose); 
    }
    else 
        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), NULL, NULL, verbose); 

    if (pid < 0)
        err_sys ("fork error"); 
    else if (pid == 0)
    {
        // child
        if (noecho)
        {
            if (verbose)
                syslog (LOG_INFO, "shutdown echo for child\n"); 

            set_noecho (STDIN_FILENO); 
        }
        
        if (execvp (argv[optind], &argv[optind]) < 0)
            err_sys ("can't execute: %s", argv[optind]); 
    }

    // parent
    syslog (LOG_INFO, "verbose = %d\n", verbose); 
    if (verbose) 
    {
        syslog (LOG_INFO, "slave name = %s\n", slave_name); 
        if (driver != NULL)
            syslog (LOG_INFO, "driver = %s\n", driver); 
    }

#if 1
    if (interactive && driver == NULL) {
        if (tty_raw (STDIN_FILENO) < 0)
            err_sys ("tty_raw error"); 
        else if (verbose)
            syslog (LOG_INFO, "tty_raw mode for parent\n"); 

        if (atexit (tty_atexit) < 0)
            err_sys ("atexit error"); 
        else if (verbose)
            syslog (LOG_INFO, "register tty_atexit ok\n"); 
    }
#endif

#if 0
    if (driver)
        do_driver (driver); 
#endif

    loop (fdm, ignoreeof, verbose); 
    exit (0); 
}
