#include "../apue.h"
#include <wchar.h> 

void do_fwide (FILE* fp, int wide)
{
  if (wide > 0)
      fwprintf (fp, L"do fwide %d\n", wide); 
  else
      fprintf (fp, "do fwide %d\n", wide); 
}

/**
 *@param: wide
 *   -1 : narrow
 *   1  : wide
 *   0  : undetermine
 */
void set_fwide (FILE* fp, int wide)
{
  int ret = fwide (fp, wide); 
  printf ("old wide = %d, new wide = %d\n", ret, wide); 
}

void get_fwide (FILE* fp)
{
    set_fwide (fp, 0); 
}

int main (int argc, char* argv[])
{
  int towide = 0; 
  FILE* fp = fopen ("abc.txt", "w+"); 
  if (fp == 0)
    err_sys ("fopen failed"); 

#if defined (USE_WCHAR)
  towide = 1;
#else
  towide = -1;  
#endif

#if defined (USE_EXPLICIT_FWIDE)
  // set wide explicitly
  set_fwide (fp, towide); 
#else
  // set wide automatically by s[w]printf
  do_fwide (fp, towide); 
#endif 

  get_fwide (fp); 

  // test set fwide after wide determined
  set_fwide (fp, towide > 0 ? -1 : 1); 

  get_fwide (fp); 

  // test output with same wide
  do_fwide (fp, towide); 
  // test output with different wide
  do_fwide (fp, towide > 0 ? -1 : 1); 

  fclose (fp); 
  return 0; 
}
