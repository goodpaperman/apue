#include "../apue.h" 
#include <sys/time.h> 
#include <time.h> 

struct timezone
{
    int     tz_minuteswest; /* of Greenwich */
    int     tz_dsttime;     /* type of dst correction to apply */
}; 

void print_tm (struct tm* t)
{
  printf ("%04d-%02d-%02d %02d:%02d:%02d (week day %d) (year day %d) (daylight saving time %d)\n", 
    t->tm_year + 1900, 
    t->tm_mon + 1, 
    t->tm_mday, 
    t->tm_hour, 
    t->tm_min, 
    t->tm_sec, 
    t->tm_wday, 
    t->tm_yday + 1, 
    t->tm_isdst); 
}

int 
main (int argc, char *argv[])
{
  int ret = 0; 
  time_t t1, t2; 
  t1 = time (&t2); 
  printf ("t1 = %ld, t2 = %ld\n", t1, t2); 

  struct timeval tv; 
  struct timezone tzp; 
  ret = gettimeofday (&tv, (void*) &tzp); 
  if (ret == -1)
    perror("gettimeofday"); 

  printf ("sizeof (suseconds_t) = %d, sizeof (struct timeval) = %d, ret %d, tv.sec = %ld, tv.usec = %ld\n", 
          sizeof (suseconds_t), sizeof (struct timeval), ret, tv.tv_sec, tv.tv_usec); 
  printf ("minuteswest = %d, dsttime = %d\n", tzp.tz_minuteswest, tzp.tz_dsttime); 

  struct tm *tm1 = gmtime (&t1); 
  struct tm *tm2 = localtime (&t2); 
  print_tm (tm1); 
  print_tm (tm2); 

  time_t t3 = mktime (tm2); 
  printf ("t3 = %ld\n", t3); 

  printf ("from asctime: %s", asctime (tm2)); 
  printf ("from ctime: %s", ctime (&t3)); 
  return 0; 
}
