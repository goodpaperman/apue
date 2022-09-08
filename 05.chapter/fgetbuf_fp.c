#include "../apue.h"
#include <stdio.h> 

int main (int argc, char* argv[])
{
  FILE* fp = NULL; 
  FILE* fp1 = fopen ("flbuf.txt", "w+"); 
  FILE* fp2 = fopen ("lnbuf.txt", "w+"); 
  FILE* fp3 = fopen ("nobuf.txt", "w+"); 
  FILE* fp4 = fopen ("unbuf.txt", "w+"); 
  
  fp = fp1; 
  if (setvbuf (fp, NULL, _IOFBF, 8192) != 0)
      err_sys ("fp (full null 8192) failed"); 
  tell_buf ("fp (full null 8192)", fp); 

#ifdef FP_CHANGE
  fp = fp2; 
#endif
  if (setvbuf (fp, NULL, _IOLBF, 3072) != 0)
      err_sys ("fp (line null 3072) failed"); 
  fputs ("fp", fp); 
  tell_buf ("fp (line null 3072)", fp); 

#ifdef FP_CHANGE
  fp = fp3; 
#endif
  if (setvbuf (fp, NULL, _IOLBF, 0) != 0)
      err_sys ("fp (line null 0) failed"); 
  fputs ("fp", fp); 
  tell_buf ("fp (line null 0)", fp); 

#ifdef FP_CHANGE
  fp = fp4; 
#endif
  if (setvbuf (fp, NULL, _IOFBF, 0) != 0)
      err_sys ("fp (full null 0) failed"); 
  tell_buf ("fp (full null 0)", fp); 

  fclose (fp1); 
  fclose (fp2); 
  fclose (fp3); 
  fclose (fp4); 
  return 0; 
}
