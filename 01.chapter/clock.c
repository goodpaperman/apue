#include <time.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/times.h> 


int main (int argc, char *argv[])
{
  clock_t start = 0, stop = 0; 
  clock_t begin = 0, end = 0; 
  start = clock (); 
  begin = times (0); 
  
#if 0
  sleep (1); 
#else
  int n = 0; 
  for (; n < 10000; ++ n)
    printf ("this is %d\n", n); 
#endif 

  end = times (0); 
  stop = clock (); 

  printf ("CLOCKS_PER_SEC = %ld, start = %ld, stop = %ld, elapsed = %ld (%f s)\n", 
    CLOCKS_PER_SEC, 
    start, 
    stop, 
    stop - start, 
    1.0 * (stop - start) / CLOCKS_PER_SEC); 

  int clk_tck = sysconf (_SC_CLK_TCK); 
  printf ("clk_tck = %d, begin = %ld, end = %ld, elapsed = %ld (%f s)\n", 
    clk_tck, 
    begin, 
    end, 
    end - begin, 
    1.0 * (end - begin) / clk_tck); 

  return 0; 
}

