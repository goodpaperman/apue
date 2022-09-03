#include "../apue.h" 
#include <errno.h> 

int main (int argc, char *argv[])
{
  char line [MAXLINE]; 
  strcpy (line, "/tmp/this_is_a_temp_file_XXXXXX"); 
  int fd = mkstemp (line); 
  if (fd == 0)
    err_sys ("mkstemp error"); 

  printf ("%s\n", line); 
  write (fd, line, strlen (line)); 
  unlink (line); 

  strcpy (line, "/tmp/that_is_a_temp_file_XXXXXX"); 
  printf ("%p: %s\n", line, line); 
  char *ptr = mktemp (line); 
  printf ("%p: %s, errno = %d\n", ptr, ptr, errno); 

  strcpy (line, "/tmp/XXXXXX_that_is_a_temp_file"); 
  ptr = mktemp (line); 
  printf ("%p: %s, errno = %d\n", ptr, ptr, errno); 
  return 0; 
}

