#include "../apue.h"

void print_envs()
{
  extern char **environ; 
  printf ("base %p\n", environ); 
  for (int i=0; environ && environ[i] != 0; ++ i)
  {
    printf ("env[%d] = [%p]%s\n", i, environ[i], environ[i]); 
  }
}

int 
main (int argc, char *argv[])
{
  print_envs ();  

  setenv ("HOME", "ME", 1); 
  printf ("\nafter set HOME:\n"); 
  print_envs (); 

  setenv ("LOGNAME", "this is a very very long user name", 1); 
  printf ("\nafter set LOGNAME:\n"); 
  print_envs (); 

  unsetenv ("PATH"); 
  printf ("\nafter unset PATH:\n"); 
  print_envs (); 

  setenv ("DISAPPEAR", "not exist before", 0); 
  printf ("\nafter set DISAPPEAR:\n"); 
  print_envs (); 

  setenv ("ADDISION", "addision adding", 0); 
  printf ("\nafter set ADDISION:\n"); 
  print_envs (); 

  return 0; 
}
