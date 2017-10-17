#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  FILE* fp = fopen ("abc.txt", "w+"); 
  if (fp == 0)
    err_sys ("fopen failed"); 

#if 1
  ret = fwide (fp, 0); 
#elif defined(USE_WCHAR)
  ret = fwide (fp, -1); 
#else
  ret = fwide (fp, 1); 
#endif
  printf ("fwide = %d\n", ret); 
#ifdef USE_WCHAR
  fwprintf (fp, L"fwide = %d\n", ret); 
#else
  fprintf (fp, "fwide = %d\n", ret); 
#endif 

#if 1
  ret = fwide (fp, 0); 
#elif defined(USE_WCHAR)
  ret = fwide (fp, -1); 
#else 
  ret = fwide (fp, 1); 
#endif 
  printf ("fwide = %d\n", ret); 

#ifdef USE_WCHAR
  fwprintf (fp, L"fwide = %d\n", ret); 
#else
  fprintf (fp, "fwide = %d\n", ret); 
#endif 

  fclose (fp); 
  return 0; 
}
