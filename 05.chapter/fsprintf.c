#include "../apue.h"
#include <wchar.h> 
#include <stdint.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  char buf[1024] = { 0 }; 
  if (argc < 4)
  {
    printf ("Usage: fsprintf str int flt\n"); 
    return -1; 
  }

  char const* str = argv[1]; 
  int n = atoi (argv[2]); 
  float f = atof (argv[3]); 
  printf ("-: %-5s, %-5d, %-5x, %-5o, %-5f\n", str, n, n, n, f); 
  printf ("+: %+5s, %+5d, %+5x, %+5o, %+5f\n", str, n, n, n, f); 
  printf (" : % 5s, % 5d, % 5x, % 5o, % 5f\n", str, n, n, n, f); 
  printf ("#: %#5s, %#5d, %#5x, %#5o, %#5f\n", str, n, n, n, f); 
  printf ("0: %05s, %05d, %05x, %05o, %05f\n", str, n, n, n, f); 
  printf ("*: %*s, %.*s, %*d, %*f, %*.*f\n", n, str, n, str, n, n, n, f, n, n, f); 
  //printf ("*: %1$*s, %1$*d, %1$*f, %1$*.1$*f\n", n, str, n, f, f); 
  //printf ("*: %1$*2$s, %1$*3$d, %1$*4$f, %1$*.1$*5$f\n", n, str, n, f, f); 
  printf ("*: %2$*1$s, %3$*1$d, %4$*1$f, %5$.*1$f\n", n, str, n, f, f); 
  printf ("': %'d, %'5.5f\n", n, f); 

  char c = -1; //'c'; 
  printf ("hh: %hhd, %hhu\n", c, c); 

  short s = -1; 
  printf ("h: %hd, %hu\n", s, s); 

  long long ll = -1; 
  printf ("ll: %lld, %llu\n", ll, ll); 

  intmax_t imt = -1; 
  uintmax_t uimt = -1; 
  printf ("j: %jd, %ju\n", imt, uimt); 
  
  size_t st = -1; 
  printf ("z: %zd, %zu\n", st, st); 

  ptrdiff_t pdt = -1; 
  printf ("t: %td, %tu\n", pdt, pdt); 

  long double pi = 3.1415926539798626; 
  printf ("L: %.20Lf\n", pi); 

  int n1= 0, n2 = 0, n3 = 0; 
  double dbl = 2.1832939029393; 
  n3 = sprintf (buf, "dbl: %.10f,%n%.10e,%n%a\n", dbl, &n1, dbl, &n2, dbl); 
  printf ("%s", buf); 
  printf ("n1 = %d, n2 = %d, n3 = %d\n", n1, n2, n3); 
  printf ("%.*s\n", n1, buf); 
  printf ("%.*s\n", n2, buf); 

  sprintf (buf, "%s some appends\n", buf); 
  printf ("after append: \n"); 
  printf ("%s", buf); 

  wchar_t const* ws = L"ni hao ma"; 
  printf ("ls: %s, %ls, %S, %p\n", ws, ws, ws, ws); 
  return 0; 
}
