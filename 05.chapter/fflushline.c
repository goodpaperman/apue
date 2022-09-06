#include "../apue.h"

int main (int argc, char* argv[])
{
  FILE* fp1 = fopen ("flbuf.txt", "w+"); 
  FILE* fp2 = fopen ("lnbuf.txt", "w+"); 
  FILE* fp3 = fopen ("nobuf.txt", "r+"); 
  if (fp1 == 0 || fp2 == 0 || fp3 == 0)
    err_sys ("fopen failed"); 

  // initialize buffer type
  // fp1 keep full buffer
  if (setvbuf (fp2, NULL, _IOLBF, 0) < 0)
      err_sys ("set line buf failed"); 

  if (setvbuf (fp3, NULL, _IONBF, 0) < 0)
      err_sys ("set no buf failed"); 

  // fill buffer
  printf ("first line to screen! "); 
  fprintf (fp1, "first line to full buf! "); 
  fprintf (fp2, "first line to line buf! "); 

  // case 1: read from line buffered FILE* and need fetch data from system
  sleep (3); 
  getchar(); 

  // fill buffer again
  printf ("last line to screen."); 
  fprintf (fp1, "last line to full buf."); 
  fprintf (fp2, "last line to line buf."); 

  // case 2: read from no buffered FILE* 
  sleep (3); 
  int ret = fgetc (fp3); 
  // give user some time to check file content
  // note no any output here to avoid repeat case 1
  sleep (10); 

  printf ("\n%c: now all over!\n", ret); 

  fclose (fp1); 
  fclose (fp2); 
  fclose (fp3); 
  return 0; 
}
