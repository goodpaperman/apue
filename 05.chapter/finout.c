#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  char ch = 0; 
  int n = 12; 
  fpos_t pos = { 0 }; 
  char const* path = "abc.txt"; 
  FILE* fp = fopen (path, "w+"); 
  if (fp == 0)
    err_sys ("fopen failed"); 

  // generate some data
  for (int i=0; i<26; ++ i)
  {
    ch = 'a' + i; 
    fwrite (&ch, 1, 1, fp); 
  }

#ifndef TEST1
  rewind (fp); 
#endif 
  fread (&ch, 1, 1, fp); 
  printf ("read %c at begin\n", ch); 

#ifndef TEST2
  fseek (fp, n, SEEK_SET); 
#endif 
  fwrite (&ch, 1, 1, fp); 
  printf ("write %c at %d\n", ch, n); 

#ifndef TEST3
  pos.__pos = 24; 
  fsetpos (fp, &pos); 
#endif
  fread (&ch, 1, 1, fp); 
  printf ("read %c at %ld\n", ch, pos.__pos); 

#ifndef TEST4
  n = 5; 
  fseek (fp, n, SEEK_SET); 
#endif 
  fwrite (&ch, 1, 1, fp); 
  printf ("write %c at %d\n", ch, n); 

#ifndef TEST5
  fflush (fp); 
#endif 
  fread (&ch, 1, 1, fp); 
  printf ("read %c\n", ch); 

  fclose (fp); 
  return 0; 
}
