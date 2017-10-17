#include "../apue.h"
#include <wchar.h> 
#include <stdio.h> 


int main (int argc, char* argv[])
{
  int a; 
#ifdef USE_LINEBUF
  scanf ("%d", &a); 
  printf ("a = %d\n", a); 
#endif 
  tell_buf ("stdin", stdin); 
  tell_buf ("stdout", stdout); 
  tell_buf ("stderr", stderr); 

  //int ret = 0; 
  char buf[BUFSIZ] = { 0 }; 
  printf ("bufsiz = %d\n", BUFSIZ); 
  setbuf (stdout, NULL); 
  tell_buf ("stdout (no)", stdout); 
  setbuf (stderr, buf); 
  tell_buf ("stderr (has)", stderr); 
  setbuf (stdin, buf); 
  //scanf ("%d", &a); 
  //tell_buf ("stdin (has in)", stdin); 
  tell_buf ("stdin (has)", stdin); 

  setvbuf (stdout, buf, _IOFBF, 512); 
  tell_buf ("stdout (full)", stdout); 
  setvbuf (stdout, NULL, _IONBF, 0); 
  tell_buf ("stdout (no)", stdout); 
  setvbuf (stdout, buf, _IOLBF, 256); 
  tell_buf ("stdout (line)", stdout); 
  setvbuf (stdout, NULL, _IOFBF, 512); 
  tell_buf ("stdout (full null)", stdout); 
  setvbuf (stdout, NULL, _IOLBF, 256); 
  tell_buf ("stdout (line null)", stdout); 
  return 0; 
}
