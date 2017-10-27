#include <stdio.h> 
#include <string.h> 

int strcmp (char const* s1, char const* s2)
{
  printf ("hack function invoked, s1: %s, s2: %s\n", s1, s2); 
  return 0; 
}
