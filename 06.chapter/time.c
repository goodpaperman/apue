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
    t->tm_wday == 0 ? 7 : t->tm_wday, 
    t->tm_yday + 1, 
    t->tm_isdst); 
}

void print_tz ()
{
  printf ("tzname[0] = %s, tzname[1] = %s, timezone = %d, daylight = %d\n", tzname[0], tzname[1], timezone, daylight); 

#if 0
  // reset tz information
  char const* tz = getenv("TZ"); 
  putenv("TZ="); 
  tzset(); 

  char tzval[1024] = { 0 }; 
  sprintf (tzval, "TZ=%s", tz);
  putenv(tzval); 
#endif
}

int 
main (int argc, char *argv[])
{
  int ret = 0; 
  time_t t1, t2; 
  print_tz (); 
  t1 = time (&t2); 
  printf ("t1 = %ld, t2 = %ld\n", t1, t2); 
  print_tz (); 

  struct timeval tv; 
  struct timezone tzp; 
  ret = gettimeofday (&tv, (void*) &tzp); 
  if (ret == -1)
    perror("gettimeofday"); 

  printf ("sizeof (suseconds_t) = %d, sizeof (struct timeval) = %d, ret %d, tv.sec = %ld, tv.usec = %ld\n", 
          sizeof (suseconds_t), sizeof (struct timeval), ret, tv.tv_sec, tv.tv_usec); 
  printf ("minuteswest = %d, dsttime = %d\n", tzp.tz_minuteswest, tzp.tz_dsttime); 
  print_tz (); 

  struct tm *tm0 = gmtime (&t1); 
  struct tm tm1 = { 0 }; 
  memcpy (&tm1, tm0, sizeof (struct tm)); 
  print_tm (&tm1); 
  print_tz (); 

  tm0 = localtime (&t2); 
  struct tm tm2 = { 0 }; 
  memcpy (&tm2, tm0, sizeof (struct tm)); 
  print_tm (&tm2); 
  print_tz (); 

  struct tm tm3 = { 0 }; 
#if 1
  memcpy (&tm3, &tm2, sizeof (struct tm)); 
#else 
  memcpy (&tm3, &tm1, sizeof (struct tm)); 
#endif

#if 1
  // tm3.tm_isdst = 0; 
  time_t t3 = mktime (&tm3); 
  printf ("t3 = %ld\n", t3); 
  print_tm (&tm3); 
  print_tz (); 
#else 
  time_t t3 = t1 + 18000; 
#endif 

  printf ("from asctime: %s", asctime (&tm1)); 
  print_tz (); 

  printf ("from ctime: %s", ctime (&t3)); 
  printf ("t1 = %ld\n", t3); 
  print_tz (); 
  return 0; 
}
