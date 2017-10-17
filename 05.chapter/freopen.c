#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  FILE* in = freopen ("stdin.txt", "r", stdin); 
  FILE* out = freopen ("stdout.txt", "w", stdout); 
  FILE* err = freopen ("stderr.txt", "a", stderr); 
  if (in == 0 || out == 0 || err == 0)
    err_sys ("freopen failed"); 

  int a = 0; 
  scanf ("%d", &a); 
  printf ("a = %d\n", a); 
  fprintf (stderr, "a = %d!\n", a); 
  return 0; 
}
