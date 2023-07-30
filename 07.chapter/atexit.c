#include "../apue.h" 

void do_dirty_work ()
{
  printf ("doing dirty works!\n"); 
}

int exit_status = 10; 
void bye ()
{
  printf ("bye, forks~\n"); 
#if 1
  exit (exit_status++);  // no effect
  printf ("after exit (2)\n"); 
#endif
#if 1
  _exit (3);  // quit and no other atexit function running anymore !
  printf ("after _exit (3)\n"); 
#endif
}

void times ()
{
  static int counter = 32; 
  printf ("times %d\n", counter--); 
}

int main ()
{
  int ret = 0; 
  ret = atexit (do_dirty_work); 
  if (ret != 0)
    err_sys ("atexit"); 

  ret = atexit (bye); 
  if (ret != 0)
    err_sys ("bye1"); 

  ret = atexit (bye); 
  if (ret != 0)
    err_sys ("bye2"); 

  for (int i=0; i<32; i++)
  {
    ret = atexit (times); 
    if (ret != 0)
      err_sys ("times"); 
  }

#if 0
  // no atexit function calls if killed by signals !
  raise (SIGKILL); 
#endif

  printf ("main is done!\n"); 
  return 0; 
}
