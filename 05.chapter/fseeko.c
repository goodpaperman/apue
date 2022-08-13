#include "../apue.h"
#include <errno.h>

int main (int argc, char* argv[])
{
  FILE* fp = fopen ("large.dat", "r"); 
  if (fp == 0)
    err_sys ("fopen failed"); 

  int i = 0; 
  off_t ret = 0; 
  off_t pos[2] = { 2u*1024*1024*1024+100 /* 2g more */, 4ull*1024*1024*1024+100 /* 4g more */ }; 
  for (i = 0; i<2; ++ i)
  {
      if (fseeko (fp, pos[i], SEEK_SET) == -1)
      {
          printf ("fseeko failed for %llu, errno %d\n", pos[i], errno); 
      }
      else 
      {
          printf ("fseeko to %llu\n", pos[i]); 
          ret = ftello (fp); 
          printf ("after fseeko: %llu\n", ret); 
      }
  }

  return 0; 
}
