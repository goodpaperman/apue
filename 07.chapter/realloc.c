#include "../apue.h" 

void* alloc (int size)
{
  char *buf = alloca (size); 
  printf ("alloca %p %d\n", buf, size); 
  return buf; 
}

int main (int argc, char *argv[])
{
  void *p1 = sbrk (0); 
  char *ptr = malloc (0), *p = 0; 
  printf ("malloc (0) = %p, %lu\n", ptr, (unsigned long)ptr/8); 

  int n = 0, i = 0; 
  for (i=1; 1; i *= 2)
  {
    p = realloc (ptr, i); 
    if (p != ptr)
    {
      printf ("base address changed %p != %p, size = %d\n", p, ptr, i); 
      ptr = p; 
      if (n++ > 3)
        break; 
    }

    ptr = p; 
  }

  p = realloc (ptr, 1); 
  ptr = p; 
  printf ("after shrink %p, %lu\n", ptr, (unsigned long) ptr/8); 
  //ptr[2] = 'a'; 
  free (ptr); 
  // crash!
  //free (ptr); 

  struct abc{
    int a; 
    double b; 
    char c[128]; 
  } *pabc; 

  pabc = calloc (sizeof (struct abc), 10); 
  // crash
  //memset (&pabc[10], 0, sizeof (struct abc)); 
  free (pabc); 
  void *p2 = sbrk (0); 
  printf ("p1 = %p, p2 = %p\n", p1, p2); 

  char *p3 = alloc (100); 
  // crash
  //p3[0] = '1'; 
  // crash
  //char *p4 = alloc (1024 * 1024 * 1024); 
  return 0; 
}
