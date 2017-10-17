#include "../apue.h"
#include <wchar.h> 

int glob = 6; 
char buf[] = "a write to stdout\n"; 

int main (int argc, char* argv[])
{
  int var = 88; 
  pid_t pid; 
  if (write (STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1)
    err_sys ("write error"); 

#if 0
  // 1.test line buffer flushed when encounter '\n'
  printf ("before fork\n"); 
#endif 

#if 0
  // 2.test line buffer flushed when buffer size reached (1024)
  for (int i=0; i<1024+1; ++ i)
    printf ("%c", 'a' + i%26); 
#endif

#if 0
  // 3. test line buffer flushed when we get input data from kernel
  printf ("before fork"); 
  char c = 0; 
  scanf ("%c", &c); 
#endif 

#if 1
  setvbuf (stdout, NULL, _IONBF, 0); 
  tell_buf ("stdout (no)", stdout); 
  setvbuf (stdout, NULL, _IOFBF, 512); 
  tell_buf ("stdout (full null)", stdout); 
  setvbuf (stdout, NULL, _IOLBF, 256); 
  tell_buf ("stdout (line null)", stdout); 
  printf ("before fork"); 
#endif

  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
  {
    glob ++; 
    var ++; 
  }
  else 
    sleep (2); 

  printf (" pid = %d, glob = %d, var = %d\n", getpid (), glob, var); 
  return 0; 
}
