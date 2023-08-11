#include "../apue.h" 


#if 1
int main (int argc, char* argv[])
#elif 0
void main (int argc, char* argv[])
#elif 0
void main ()
#elif 0
int main()
#else
int main (int argc, char* argv[], char* envp[])
#endif
{
  printf ("hello this is nake main!\n"); 
#if 1
  return 1; 
#elif 0
  return; 
#elif 0
  exit (2); 
#else
  int i=0; 
  for (i=0; envp[i]!=NULL; ++i)
      printf ("%d: %s\n", i, envp[i]); 
#endif
}
