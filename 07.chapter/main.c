#include "../apue.h" 


#if 1
int main (int argc, char* argv[])
#elif 0
void main (int argc, char* argv[])
#elif 0
void main ()
#else
int main()
#endif
{
  printf ("hello this is nake main!\n"); 
#if 1
  return 1; 
#elif 0
  return; 
#elif 0
  exit (2); 
#endif
}
