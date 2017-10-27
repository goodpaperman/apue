#include <stdio.h> 
#include <string.h> 

int strcmp (char const* s1, char const* s2)
{
  printf ("hack function invoked, s1: %s, s2: %s\n", s1, s2); 
  //return 0; 
  //return strcmp (s1, s2); 
  char const* p1=s1, *p2=s2; 
  for (; p1 && *p1 && p2 && *p2; ++p1, ++p2)
  {
    if (*p1 != *p2)
      return *p1 - *p2; 
  }

  if (p1 && *p1)
    return 1; 

  if (p2 && *p2) 
    return -1; 

  return 0; 
}
