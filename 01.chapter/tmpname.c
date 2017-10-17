#include <stdio.h> 

int main (int argc, char *argv[])
{
  int i = 0; 
  for (; i<TMP_MAX+10; ++ i)
    printf ("%6d: %s\n", i+1, tmpnam (0)); 
  return 0; 
}
