#include "../apue.h" 

#ifdef USE_ENV
int main (int argc, char *argv[], char *env[])
#else 
int main (int argc, char *argv[])
#endif
{
#ifndef USE_ENV
  extern char **environ; 
  char **env = environ; 
#endif
  for (int i=0; env[i]; ++ i)
    printf ("env[%d] = %s\n", i, env[i]); 

  return 0; 
}
