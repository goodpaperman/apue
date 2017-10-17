#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  char pad[128] = { 0 }; 
  char buf_small[16] = { 0 }; 
  char buf_huge[128] = { 0 }; 

  if (argc < 2)
  {
    printf ("Usage: fprintf string\n"); 
    return 1; 
  }

  ret = sprintf (buf_small, "the string is %s\n", argv[1]); 
  printf ("sprintf small buf result: [%d] %s, len = %u, pad = %s", ret, buf_small, strlen (buf_small), pad); 
  ret = sprintf (buf_huge, "the string is %s\n", argv[1]); 
  printf ("sprintf huge buf result: [%d] %s, len = %u\n", ret, buf_huge, strlen (buf_huge)); 

  ret = snprintf (buf_small, 10,  "%s\n", argv[1]); 
  printf ("sprintf small buf result: [%d] %s, len = %u, pad = %s", ret, buf_small, strlen (buf_small), pad); 
  ret = snprintf (buf_huge, 100, "%s\n", argv[1]); 
  printf ("sprintf huge buf result: [%d] %s, len = %u\n", ret, buf_huge, strlen (buf_huge)); 
  return 0; 
}
