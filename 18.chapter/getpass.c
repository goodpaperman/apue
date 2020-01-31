#include "../apue.h"
#include <unistd.h>

#include <signal.h>
#include <termios.h>

#define MAX_PASS_LEN 80

char* my_getpass (char const* prompt)
{
  static char buf[MAX_PASS_LEN+1]; 
  char *ptr; 
  int c; 

  FILE *fp; 
  if ((fp = fopen(ctermid (NULL), "r+")) == NULL)
    return NULL; 

  setbuf (fp, NULL); 

  sigset_t sig, osig; 
  sigemptyset(&sig); 
  sigaddset (&sig, SIGINT); 
  sigaddset (&sig, SIGTSTP); 
  sigprocmask (SIG_BLOCK, &sig, &osig); 

  struct termios ts, ots; 
  tcgetattr (fileno(fp), &ts); 
  ots = ts; 

  // hide input
  ts.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL); 
  tcsetattr (fileno(fp), TCSAFLUSH, &ts); 
  fputs (prompt, fp); 

  ptr = buf; 
  while ((c = getc (fp)) != EOF && c != '\n')
    if (ptr < &buf[MAX_PASS_LEN])
      *ptr ++ = c; 

  *ptr = 0; 
#if 1
  // need this to reprint \n ignored by shutdown echo
  putc ('\n', fp); 
#endif

  tcsetattr (fileno(fp), TCSAFLUSH, &ots); 
  sigprocmask (SIG_SETMASK, &osig, NULL); 
  fclose (fp); 
  return buf; 
}

#define GETPASS my_getpass
//#define GETPASS getpass

int main ()
{
  char *ptr; 
  if ((ptr = GETPASS ("Enter Password:")) == NULL)
    err_sys ("getpass error"); 

  printf ("password: %s\n", ptr); 
  while (*ptr != 0)
    *ptr ++ = 0; 

  return 0; 
}
