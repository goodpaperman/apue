#include "../apue.h"
#include <stdio.h> 

int main (int argc, char* argv[])
{
  char buf1[BUFSIZ] = { 0 }; 
  char buf2[BUFSIZ] = { 0 }; 
  char *buf = buf1; 
  printf ("BUFSIZ = %d, address1 %p, address2 %p\n", BUFSIZ, buf1, buf2); 
  FILE* fp = fopen ("unbuf.txt", "w+"); 
  
   setbuf (fp, buf); 
  tell_buf ("fp (full)", fp); 

#ifdef BUF_CHAGE
  buf = buf2; 
#else
  buf = NULL; 
  setbuf (fp, NULL); 
  // tell_buf ("fp (no)", fp); 
#endif
  if (setvbuf (fp, buf, _IOLBF, 4096) != 0)
      err_sys ("fp (line null 4096) failed"); 
  fputs ("fp", fp); 
  tell_buf ("fp (line null 4096)", fp); 

#ifdef BUF_CHAGE
  buf = buf1; 
#else
  setbuf (fp, NULL); 
  // tell_buf ("fp (no)", fp); 
#endif
  if (setvbuf (fp, buf, _IOFBF, 3072) != 0)
      err_sys ("fp (full null 3072) failed"); 
  tell_buf ("fp (full null 3072)", fp); 

#ifdef BUF_CHAGE
  buf = buf2; 
#else
  setbuf (fp, NULL); 
  // tell_buf ("fp (no)", fp); 
#endif
  if (setvbuf (fp, buf, _IOLBF, 2048) != 0)
      err_sys ("fp (line null 2048) failed"); 
  fputs ("fp", fp); 
  tell_buf ("fp (line null 2048)", fp); 

#ifdef BUF_CHAGE
  buf = buf1; 
#else 
  setbuf (fp, NULL); 
  // tell_buf ("fp (no)", fp); 
#endif
  if (setvbuf (fp, buf, _IOFBF, 1024) != 0)
      err_sys ("fp (full null 1024) failed"); 
  tell_buf ("fp (full null 1024)", fp); 

  fclose (fp); 
  return 0; 
}
