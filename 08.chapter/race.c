#include "../apue.h" 

static void charatatime (char *); 

int main (void)
{
  pid_t pid; 
  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
  {
    //sleep (10); 
    charatatime ("output from child\n"); 
  }
  else 
    charatatime ("output from parent\n"); 

  exit (0); 
}

static void charatatime (char *str)
{
  char *ptr; 
  int c; 
  setbuf (stdout, NULL); 
  for (ptr = str; (c = *ptr++) != 0; )
  {
    putc (c, stdout); 
    sleep (1); 
  }
}
