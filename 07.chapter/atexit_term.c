#include "../apue.h" 

extern void bye ();
void do_dirty_work ()
{
  printf ("doing dirty works!\n"); 
#if 0
  int ret = atexit (bye); 
  if (ret != 0)
      err_sys ("bye2"); 
#endif
}

void bye ()
{
  printf ("bye, forks~\n"); 
#if 1
  int ret = atexit (do_dirty_work); 
#else
  int ret = atexit (bye); 
#endif
  if (ret != 0)
      err_sys ("do_dirty_work"); 
}

int main ()
{
  int ret = 0; 
  ret = atexit (bye); 
  if (ret != 0)
    err_sys ("bye"); 

  printf ("main is done!\n"); 
  return 0; 
}
