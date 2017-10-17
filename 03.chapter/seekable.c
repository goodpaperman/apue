#include <stdio.h> 
#include <unistd.h> 

int main (int argc, char *argv[])
{
  int ret = 0; 
  ret = lseek (STDIN_FILENO, 0, SEEK_CUR); 
  if (ret == -1)
    printf ("cannot seek\n"); 
  else 
    printf ("seekable!\n"); 

  return ret; 
}
