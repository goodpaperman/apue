#include "../apue.h" 
#include <sys/time.h> 
#include <time.h> 

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

void my_strftime (char const* fmt, struct tm* t)
{
  char buf[64] = { 0 }; 
  int ret = strftime (buf, sizeof (buf), fmt, t); 
  printf ("[%02d] '%s': %s\n", ret, fmt, buf); 
}

int 
main (int argc, char *argv[])
{
  int ret = 0; 
  time_t now = time (NULL); 
  printf ("now = %ld\n", now); 

  struct tm *t = localtime (&now); 
  print_tm (t); 
  printf ("year group:\n");
  my_strftime ("%Y", t); 
  my_strftime ("%C", t); 
  my_strftime ("%y", t); 
  my_strftime ("%G", t); 
  my_strftime ("%g", t); 

  printf ("month group:\n"); 
  my_strftime ("%m", t); 
  my_strftime ("%b", t); 
  my_strftime ("%h", t); 
  my_strftime ("%B", t); 

  printf ("day group:\n");
  my_strftime ("%w", t); 
  my_strftime ("%u", t); 
  my_strftime ("%a", t); 
  my_strftime ("%A", t); 
  my_strftime ("%d", t); 
  my_strftime ("%e", t); 
  my_strftime ("%j", t); 

  printf ("week group:\n"); 
  my_strftime ("%U", t); 
  my_strftime ("%W", t); 
  my_strftime ("%V", t); 

  printf ("date group\n"); 
  my_strftime ("%D", t); 
  my_strftime ("%x", t); 
  my_strftime ("%F", t); 

  printf ("time group\n"); 
  my_strftime ("%H", t); 
  my_strftime ("%k", t); 
  my_strftime ("%I", t); 
  my_strftime ("%l", t); 
  my_strftime ("%M", t); 
  my_strftime ("%S", t); 
  my_strftime ("%T", t); 
  my_strftime ("%X", t); 
  my_strftime ("%R", t); 
  my_strftime ("%p", t); 
  my_strftime ("%P", t); 
  my_strftime ("%r", t); 
  my_strftime ("%c", t); 
  my_strftime ("%s", t); 

  printf ("timezone group\n"); 
  my_strftime ("%z", t); 
  my_strftime ("%Z", t); 

  printf ("common group\n"); 
  my_strftime ("%n", t); 
  my_strftime ("%t", t); 
  my_strftime ("%%", t); 
  return 0; 
}
