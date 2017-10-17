#include "../apue.h"
#include <wchar.h> 
#include <stdint.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  char buf[1024] = { 0 }; 
  sprintf (buf, "%d %.8f %s %d\n", 12, 3.1415, "nihao", 42); 

  double f = 0.0; 
  int n = 0, m = 0; 
  int pos1 = 0, pos2 = 0; 
  char str[128] = { 0 }; 
  
  ret = sscanf (buf, "%d %lf %n%s %n%d\n", &n, &f, &pos1, str, &pos2, &m); 
  printf ("buf (%d) : \n%s", ret, buf); 
  printf ("%d %.8lf [%d]%5s [%d]%d\n", n, f, pos1, str, pos2, m); 

  n = m = 0; 
  pos1 = pos2 = 0; 
  memset (str, 0, 128); 
  ret = sscanf (buf, "%d %*f %n%s %n%d\n", &n , &pos1, str, &pos2, &m); 
  printf ("buf (%d) : \n%s", ret, buf); 
  printf ("%d [%d]%5s [%d]%d\n", n, pos1, str, pos2, m); 

  n = m = 0; 
  pos1 = pos2 = 0; 
  memset (str, 0, 128); 
  ret = sscanf (buf, "%d %lf %n%[a-zA-Z0-9] %n%d\n", &n , &f, &pos1, str, &pos2, &m); 
  printf ("buf (%d) : \n%s", ret, buf); 
  printf ("%d %.8lf [%d]%5s [%d]%d\n", n, f, pos1, str, pos2, m); 

/*
  double *pf = 0; 
  char *pstr = 0; 
  ret = sscanf (buf, "%d %mlf %n%ms %n%d\n", &n, &pf, &pos1, &pstr, &pos2, &m); 
  printf ("buf [%d] : \n%s", ret, buf); 
  printf ("%d %.8lf [%d]%s [%d]%d\n", n, *pf, pos1, *pstr, pos2, m); 
  free (pf); 
  free (pstr); 
*/

  n = m = 0; 
  pos1 = pos2 = 0; 
  memset (str, 0, 128); 
  ret = sscanf (buf, "%3$d %5$lf %1$n%6$[a-zA-Z0-9] %2$n%4$d\n", &pos1, &pos2, &n , &m, &f, str); 
  printf ("buf (%d) : \n%s", ret, buf); 
  printf ("%d %.8lf [%d]%5s [%d]%d\n", n, f, pos1, str, pos2, m); 

  n = m = 0; 
  pos1 = pos2 = 0; 
  memset (str, 0, 128); 
  ret = sscanf (buf, "%3$d %5$lf %1$n%6$[a-zA-Z0-9] %2$n%4$d\n", &pos1, &pos2, &n , &m, &f, str); 
  printf ("buf (%d) : \n%s", ret, buf); 
  printf ("%d %.8lf [%d]%5s [%d]%d\n", n, f, pos1, str, pos2, m); 

  n = m = pos1 = pos2 = 0; 
  ret = sscanf ("0x123 0123 123", "%i %i %i", &n, &m, &pos1); 
  printf ("%d: %#x %#o %#d\n", ret, n, m, pos1); 

  n = m = pos1 = pos2 = 0; 
  ret = sscanf ("0x123 0123 123", "%d %d %d", &n, &m, &pos1); 
  printf ("%d: %#x %#o %#d\n", ret, n, m, pos1); 
  return 0; 
}
