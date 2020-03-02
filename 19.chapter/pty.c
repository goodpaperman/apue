#if !defined(__sun__) && !defined(sun)
#include "../apue.h"
#else 
#include <signal.h>
//#include <stdio.h>
#include <stdlib.h>
#endif
#include "../pty_fun.h"
#include "../tty.h"
#include <termios.h>
#include <errno.h>
#include <poll.h>
#include <syslog.h>
#include <sys/socket.h>

#define BUFFSIZE 512
#define USE_SIGTERM
#define USE_SIGWINCH


#ifdef __linux__
#define OPTSTR "+d:einv"
#else
#define OPTSTR "d:einv"
#endif

#ifdef USE_SIGTERM
static volatile sig_atomic_t sigcaught; 

static void sig_term (int signo)
{
    sigcaught = 1; 
}
#endif

static int g_pty = -1; 
static pid_t g_pid = -1; 

#ifdef USE_SIGWINCH
static void sig_winch (int signo)
{
    struct winsize sz; 
    if (ioctl (STDIN_FILENO, TIOCGWINSZ, (char *)&sz) < 0)
    {
        syslog (LOG_INFO, "TIOCGWINSZ failed\n"); 
        return; 
    }

    syslog (LOG_INFO, "SIGWINCH: %d, %d\n", sz.ws_row, sz.ws_col); 
    if (g_pty != -1 && g_pid != -1)
    {
        if (ioctl (g_pty, TIOCSWINSZ, (char *)&sz) < 0)
        {
            syslog (LOG_INFO, "TIOCSWINSZ failed\n"); 
            return; 
        }

        syslog (LOG_INFO, "set pty win size ok\n"); 
        //kill (g_pid, SIGWINCH); 
        //syslog (LOG_INFO, "kill SIGWINCH ok\n"); 
    }

    signal (SIGWINCH, sig_winch); 
}
#endif


#ifdef USE_POLL

void loop (int ptym, int ignoreeof, int verbose)
{
    int ret = 0; 
    char line[BUFFSIZE]; 
	struct pollfd pfd[2]; 
	pfd[0].fd = STDIN_FILENO; 
	pfd[0].events = POLLIN;
	pfd[1].fd = ptym; 
	pfd[1].events = POLLIN; //| POLLHUP;   
#  ifdef USE_SIGWINCH
    g_pty = ptym; 
    if (signal (SIGWINCH, sig_winch) == SIG_ERR)
    {
        syslog (LOG_INFO, "signal for SIGWINCH failed\n"); 
        return; 
    }
    else if (verbose)
        syslog (LOG_INFO, "setup sig_winch ok\n"); 
#  endif

    while (1) { 
	    ret = poll (pfd, 2, -1); 
	    if (ret == -1) { 
            syslog (LOG_INFO, "poll error"); 
            break; 
        } else if (ret == 0){ 
            if (verbose)
		        syslog (LOG_INFO, "poll over but no event\n"); 

            continue; 
        }  

        if (pfd[0].revents & POLLIN) { 
            if (verbose)
                syslog (LOG_INFO, "poll stdin\n"); 

            // has input, redirect to pipe
            ret = read (STDIN_FILENO, line, sizeof (line)); 
            if (ret < 0) {
                syslog (LOG_INFO, "read stdin error"); 
                if (errno == EINTR && !sigcaught)
                    continue; 

                break;  
            }
            else if (ret == 0) { 
                if (verbose)
                    syslog (LOG_INFO, "read end\n"); 
                break; 
            }
            else {
                ret = write (ptym, line, ret); 
                if (ret < 0)
                {
                    syslog (LOG_INFO, "write pipe error"); 
                    break; 
                }
                else if (ret == 0) { 
                    if (verbose)
                        syslog (LOG_INFO, "write end\n"); 
                    break; 
                }
                //else 
                //    printf ("write pipe %d\n", ret); 
            }
        }

        if (pfd[1].revents & POLLIN) { 
            if (verbose)
                syslog (LOG_INFO, "poll pty in\n"); 
            // has output, redirect to stdout
            ret = read (pfd[1].fd, line, sizeof (line)); 
            if (ret < 0)
            {
                syslog (LOG_INFO, "read pty error"); 
                break; 
            }
            else if (ret == 0) { 
                if (verbose)
                    syslog (LOG_INFO, "read end\n"); 
                break; 
            }
            else { 
                ret = write (STDOUT_FILENO, line, ret); 
                if (ret < 0)
                {
                    syslog (LOG_INFO, "write stdout error"); 
                    break; 
                }
                else if (ret == 0) { 
                    if (verbose)
                        syslog (LOG_INFO, "write end\n"); 
                    break; 
                }
                //else { 
                //    printf ("write %d\n", ret); 
                //}
            }
        }

        if (pfd[1].revents & POLLHUP) {
            syslog (LOG_INFO, "poll pty hup\n"); 
            break; 
        }
    }
}

#else // USE_POLL -> use fork

void loop (int ptym, int ignoreeof, int verbose)
{
    pid_t child; 
    int nread; 
    char buf[BUFFSIZE]; 

    if ((child = fork ()) < 0) {
        //err_sys ("fork error"); 
        //printf ("before 2nd fork\n"); 
        syslog (LOG_INFO, "fork error"); 
        exit (0); 
    }
    else if (child == 0)
    {
        // child
        //printf ("in 2nd child\n"); 
        // restore ppid to avoid parent exit
        pid_t ppid = getppid (); 
        for (;;) 
        {
            if ((nread = read (ptym, buf, BUFFSIZE)) <= 0)
            {
                if (verbose)
                    syslog (LOG_INFO, "read pty master failed, errno %d\n", errno); 

                break; 
            }
            else if (verbose)
                syslog (LOG_INFO, "read pty master %d\n", nread); 

#  if defined(__sun__) || defined(sun)
            if (write (STDOUT_FILENO, buf, nread) != nread) {
#  else
            if (writen (STDOUT_FILENO, buf, nread) != nread) {
#  endif
                //err_sys ("writen error to stdout"); 
                syslog (LOG_INFO, "writen error to stdout"); 
                exit (0); 
            }
            else if (verbose)
                syslog (LOG_INFO, "write stdout %d\n", nread); 
        }

#  ifdef USE_SIGTERM
        if (verbose)
            syslog (LOG_INFO, "send SIGTERM to parent to notify our exit\n"); 

        kill (ppid, SIGTERM); 
#  endif

        exit (0); 
    }

    if (verbose)
        syslog (LOG_INFO, "%u fork %u ok\n", getpid (), child); 

    // parent
    //printf ("in 2nd parent\n"); 
#  ifdef USE_SIGTERM
    if (signal(SIGTERM, sig_term) == SIG_ERR) {
        //err_sys ("signal error for SIGTERM"); syslog (LOG_INFO, "signal error for SIGTERM"); 
        exit (0); 
    }
    else if (verbose)
        syslog (LOG_INFO, "setup SIGTERM handler\n"); 
#  endif

#  ifdef USE_SIGWINCH
    g_pty = ptym; 
    if (signal (SIGWINCH, sig_winch) == SIG_ERR)
    {
        syslog (LOG_INFO, "signal for SIGWINCH failed\n"); 
        return; 
    }
    else if (verbose)
        syslog (LOG_INFO, "setup sig_winch ok\n"); 
#  endif


    for (;;) 
    {
        if ((nread = read (STDIN_FILENO, buf, BUFFSIZE)) < 0)
        {
            syslog (LOG_INFO, "read error %d from stdin", errno); 
            if (errno == EINTR && !sigcaught)
                continue; 

            break;
        }
        else if (nread == 0)
        {
            if (verbose)
                syslog (LOG_INFO, "read stdin end\n"); 

            break; 
        }
        else if (verbose)
            syslog (LOG_INFO, "read %d from stdin\n", nread); 

#  if defined(__sun__) || defined(sun)
        if (write (ptym, buf, nread) != nread) {
#  else
        if (writen (ptym, buf, nread) != nread) {
#  endif
            //err_sys ("writen error to master pty"); 
            syslog (LOG_INFO, "writen error to master pty"); 
            exit (0); 
        }
        else if (verbose)
            syslog (LOG_INFO, "write pty master %d ok\n", nread); 
    }

#  ifdef USE_SIGTERM
    if (sigcaught == 0)
    {
        if (ignoreeof == 0)
        {
            if (verbose)
                syslog (LOG_INFO, "no SIGTERM sent from child, try kill it\n"); 

            kill (child, SIGTERM); 
        }
        else if (verbose)
            syslog (LOG_INFO, "ignore eof, exit\n"); 
    }
    else if (verbose)
        syslog (LOG_INFO, "has caught SIGTERM, exit\n"); 
#  endif
}
#endif

static void set_noecho (int fd)
{
    struct termios term; 
    if (tcgetattr (fd, &term) < 0) {
        //err_sys ("tcgetattr error"); 
        syslog (LOG_INFO, "tcgetattr error"); 
        exit (0); 
    }

    term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); 
    //term.c_oflag &= ~(ONLCR); 

    if (tcsetattr (fd, TCSANOW, &term) < 0) {
        //err_sys ("tcsetattr error"); 
        syslog (LOG_INFO, "tcsetattr error"); 
        exit (0); 
    }
}

static void do_driver (char *driver)
{
    pid_t child; 
    int pipe[2]; 
    if (socketpair (AF_UNIX, SOCK_STREAM, 0, pipe) < 0)
        err_sys ("can't create stream pipe"); 

    if ((child = fork ()) < 0) 
        err_sys ("fork error"); 
    else if (child == 0)
    {
        close (pipe[1]); 
        if (dup2 (pipe[0], STDIN_FILENO) != STDIN_FILENO)
            err_sys ("dup2 error to stdin"); 
        if (dup2 (pipe[0], STDOUT_FILENO) != STDOUT_FILENO)
            err_sys ("dup2 error to stdout"); 
        if (pipe[0] != STDIN_FILENO && pipe[0] != STDOUT_FILENO)
            close (pipe[0]); 

        execlp (driver, driver, (char *)0); 
        err_sys ("execlp error for: %s", driver); 
    }

    close (pipe[0]); 
    if (dup2 (pipe[1], STDIN_FILENO) != STDIN_FILENO)
        err_sys ("dup2 error to stdin"); 
    if (dup2 (pipe[1], STDOUT_FILENO) != STDOUT_FILENO)
        err_sys ("dup2 error to stdout"); 
    if (pipe[1] != STDIN_FILENO && pipe[1] != STDOUT_FILENO)
        close(pipe[1]); 

    // after do_driver, 
    // we read from driver output; 
    //    write to driver input.
}

int main (int argc, char *argv[])
{
    int fdm, c, ignoreeof, interactive, noecho, verbose; 
    pid_t pid; 
    char *driver; 
    char slave_name[20]; 
    struct termios orig_termios; 
    struct winsize size; 
    openlog ("pty", LOG_CONS | LOG_PID, 0); 

    interactive = isatty (STDIN_FILENO); 
    syslog (LOG_INFO, "is stdin a tty ? %s\n", interactive ? "true" : "false"); 
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
                //err_quit ("unrecongnized option: -%c", optopt); 
                syslog (LOG_INFO, "unrecongnized option: -%c", optopt); 
                return -1; 
                break; 
        }
    }

    if (optind >= argc) {
        // no program and arg following ?
        //err_quit ("usage: pty [ -d driver -einv] program [ arg ... ]"); 
        syslog (LOG_INFO, "usage: pty [ -d driver -einv] program [ arg ... ]"); 
        return -1; 
    }

    //printf ("before fork\n"); 
    if (interactive) 
    {
        if (tcgetattr (STDIN_FILENO, &orig_termios) < 0) {
            //err_sys ("tcgetattr error on stdin"); 
            syslog (LOG_INFO, "tcgetattr error on stdin"); 
            exit (0); 
        }
        else if (verbose)
            syslog (LOG_INFO, "tcgetattr for current tty ok\n"); 

        if (ioctl (STDIN_FILENO, TIOCGWINSZ, (char *) &size) < 0) {
            //err_sys ("TIOCGWINSZ error"); 
            syslog (LOG_INFO, "TIOCGWINSZ error"); 
            exit (0); 
        }
        else if (verbose)
            syslog (LOG_INFO, "TIOCGWINSZ for currrent tty ok, %d, %d\n", size.ws_row, size.ws_col); 

        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), &orig_termios, 
#if !defined(__sun__) && !defined(sun)
        &size, 
#endif
        verbose); 
    }
    else 
        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), NULL, 
#if !defined(__sun__) && !defined(sun)
        NULL, 
#endif
        verbose); 

    //printf ("after pty_fork %d\n", pid); 
    if (pid < 0) {
        //err_sys ("fork error"); 
        syslog (LOG_INFO, "fork error"); 
        exit (0); 
    }
    else if (pid == 0)
    {
        // child
        //printf ("in child\n"); 
        if (noecho)
        {
            if (verbose)
                syslog (LOG_INFO, "shutdown echo for child\n"); 

            set_noecho (STDIN_FILENO); 
        }
        
        if (execvp (argv[optind], &argv[optind]) < 0) {
            //err_sys ("can't execute: %s", argv[optind]); 
            syslog (LOG_INFO, "can't execute: %s", argv[optind]); 
            exit (0); 
        }
    }

    // parent
    //printf ("in parent\n"); 
    //syslog (LOG_INFO, "verbose = %d\n", verbose); 
    g_pid = pid; 
    if (verbose) 
    {
        syslog (LOG_INFO, "slave name = %s\n", slave_name); 
        if (driver != NULL)
            syslog (LOG_INFO, "driver = %s\n", driver); 
    }

    if (interactive && driver == NULL) {
        if (tty_raw (STDIN_FILENO) < 0) {
            //err_sys ("tty_raw error"); 
            syslog (LOG_INFO, "tty_raw error"); 
            exit (0); 
        }
        else if (verbose)
            syslog (LOG_INFO, "tty_raw mode for parent\n"); 

        if (atexit (tty_atexit) < 0) {
            //err_sys ("atexit error"); 
            syslog (LOG_INFO, "atexit error"); 
        }
        else if (verbose)
            syslog (LOG_INFO, "register tty_atexit ok\n"); 
    }

#if 1
    if (driver)
        do_driver (driver); 
#endif

    loop (fdm, ignoreeof, verbose); 
    closelog (); 
    exit (0); 
}
