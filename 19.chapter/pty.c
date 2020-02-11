#include "../apue.h"
#include "../pty_fun.h"
#include "../tty.h"
#include <termios.h>

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

void loop (int ptym, int ignoreeof)
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
                break; 

            if (writen (ptym, buf, nread) != nread)
                err_sys ("writen error to master pty"); 
        }

        if (ignoreeof == 0)
            kill (getppid (), SIGTERM); 

        exit (0); 
    }

    // parent
    if (signal(SIGTERM, sig_term) == SIG_ERR)
        err_sys ("signal error for SIGTERM"); 

    for (;;) 
    {
        if ((nread = read (ptym, buf, BUFFSIZE)) <= 0)
            break; 
        if (writen (STDOUT_FILENO, buf, nread) != nread)
            err_sys ("writen error to stdout"); 
    }

    if (sigcaught == 0)
        kill (child, SIGTERM); 
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
        err_quit ("usage: pty [ -d driver -einv] program [ arg ... ]"); 

    if (interactive) 
    {
        if (tcgetattr (STDIN_FILENO, &orig_termios) < 0)
            err_sys ("tcgetattr error on stdin"); 
        if (ioctl (STDIN_FILENO, TIOCGWINSZ, (char *) &size) < 0)
            err_sys ("TIOCGWINSZ error"); 

        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), &orig_termios, &size, NULL); 
    }
    else 
        pid = pty_fork (&fdm, slave_name, sizeof (slave_name), NULL, NULL, NULL); 

    if (pid < 0)
        err_sys ("fork error"); 
    else if (pid == 0)
    {
        // child
        if (noecho)
            set_noecho (STDIN_FILENO); 
        
        if (execvp (argv[optind], &argv[optind]) < 0)
            err_sys ("can't execute: %s", argv[optind]); 
    }

    // parent
    if (verbose) 
    {
        fprintf (stderr, "slave name = %s\n", slave_name); 
        if (driver != NULL)
            fprintf (stderr, "driver = %s\n", driver); 
    }

    if (interactive && driver == NULL) {
        if (tty_raw (STDIN_FILENO) < 0)
            err_sys ("tty_raw error"); 
        if (atexit (tty_atexit) < 0)
            err_sys ("atexit error"); 
    }

#if 0
    if (driver)
        do_driver (driver); 
#endif

    loop (fdm, ignoreeof); 
    exit (0); 
}
