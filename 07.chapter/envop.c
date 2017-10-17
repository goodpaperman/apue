#include "../apue.h" 

void echo_env (char const* key)
{
  char const* val = getenv (key); 
  if (val == NULL)
    printf ("%s not found\n", key); 
  else 
    printf ("%s = %s\n", key, val); 
}

int main (int argc, char *argv[])
{
  if (argc < 3)
    err_sys ("Usage: %s env value\n", argv[0]); 

  char const* key = argv[1]; 
  echo_env (key); 

  char *newval = argv[2]; 
  char buf[256] = { 0 }; 
  sprintf (buf, "%s=%s", key, newval); 
  int ret = putenv (buf); 
  if (ret != 0)
    err_sys ("putenv"); 

  echo_env (key); 

  strcat (buf, " (deleted)"); 
  echo_env (key); 

  ret = unsetenv (key); 
  if (ret != 0)
    printf ("unsetenv %s failed\n", key); 

  echo_env (key); 

  ret = unsetenv (key); 
  if (ret != 0)
    printf ("unsetenv %s failed\n", key); 

  echo_env (key); 

  ret = setenv (key, newval, 0); 
  if (ret != 0)
    printf ("setenv %s failed\n", key); 

  echo_env (key); 

  ret = setenv (key, "1", 0); 
  if (ret != 0)
    printf ("setenv %s failed\n", key); 

  echo_env (key); 

  ret = setenv (key, "2", 1); 
  if (ret != 0)
    printf ("setenv %s failed\n", key); 

  echo_env (key); 
  ret = clearenv (); 
  if (ret != 0)
    printf ("clearenv failed\n"); 

  extern char **environ; 
  printf ("environ = %p\n", environ); 
  echo_env (key); 
  return 0; 
}
