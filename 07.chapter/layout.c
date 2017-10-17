#include "../apue.h" 
#if 0
#include <dlfcn.h> 
#endif 

int data1 = 2; 
int data2 = 3; 
int data3; 
int data4; 

int main (int argc, char *argv[])
{
  char buf1[1024] = { 0 }; 
  char buf2[1024] = { 0 }; 
  char *buf3 = malloc(1024); 
  char *buf4 = malloc(1024); 
  printf ("onstack %p (%lu), %p (%lu)\n", 
    buf1, 
    (unsigned long)buf1, 
    buf2, 
    (unsigned long)buf2); 

  extern char ** environ; 
  printf ("env %p\n", environ); 
  for (int i=0; environ[i] != 0; ++ i)
    printf ("env[%d] %p\n", i, environ[i]); 

  printf ("arg %p\n", argv); 
  for (int i=0; i < argc; ++ i)
    printf ("arg[%d] %p\n", i, argv[i]); 
  printf ("onheap %p (%lu), %p (%lu)\n", 
    buf3, 
    (unsigned long)buf3, 
    buf4, 
    (unsigned long)buf4); 

  free (buf3); 
  free (buf4); 

  printf ("on init %p (%lu), %p (%lu)\n", 
    &data1, 
    (unsigned long)&data1, 
    &data2, 
    (unsigned long)&data2); 

  printf ("on bss %p (%lu), %p (%lu)\n", 
    &data3, 
    (unsigned long)&data3, 
    &data4, 
    (unsigned long)&data4); 

#if 0
  void *base = dlopen (NULL, RTLD_LAZY); 
  printf ("base %p (%lu)\n", 
    base, 
    (unsigned long)base); 

  if (base)
    dlclose (base); 
#endif 

  return 0; 
}
