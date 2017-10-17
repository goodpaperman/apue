#include <stdio.h> 

extern char **environ; 
int call_func ()
{
  int i = 0; 
  // argv is located nest to envion, 
  // and envion address is large than argv at top of stack !
  char ** argv = (char **)((char *)environ - 8); 
  printf ("argv = %p\n", argv); 
  for (i=0; argv[i] != 0; ++ i)
  {
    printf ("argv[%d] = %s\n", i, argv[i]); 
  }

  return i; 
}

int main (int argc, char *argv[])
{
  printf ("argc = %d, argv = %p\n", argc, argv); 
  printf ("envion = %p\n", environ); 
  return call_func (); 
}
