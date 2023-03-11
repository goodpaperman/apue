#include "../apue.h" 
#include <sys/time.h> 
#include <time.h> 

void print_tm (struct tm* t)
{
  printf ("%04d-%02d-%02d %02d:%02d:%02d (week day %d) (year day %d) (day time saving %d)\n", 
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
  if (argc < 2)
  {
    printf ("Usage: %s [+/-] [N[Y/M/D/H/m/S/w/y]]\n", argv[0]); 
    return 0; 
  }

  int ret = 0; 
  time_t now = time (NULL); 
  printf ("sizeof (time_t) = %d, now = %ld\n", sizeof(time_t), now); 

  struct tm *tm_now = localtime (&now); 
  print_tm (tm_now); 

  int shift = 0; 
  char *endptr = 0; 
  shift = strtol (argv[1], &endptr, 10); 
  switch (*endptr)
  {
    case 'Y':
      tm_now->tm_year += shift; 
      break; 
    case 'M':
      tm_now->tm_mon += shift; 
      break; 
    case 'D':
    case 'd':
      tm_now->tm_mday += shift; 
      break; 
    case 'H':
    case 'h':
      tm_now->tm_hour += shift; 
      break; 
    case 'm':
      tm_now->tm_min += shift; 
      break; 
    case 's':
    case 'S':
      tm_now->tm_sec += shift; 
      break; 
    /* 
     * tm_wday & tm_yday is ignored normally, 
     * here just do a test !!
     */
    case 'w':
    case 'W':
      tm_now->tm_wday += shift; 
      break; 
    case 'y':
      tm_now->tm_yday += shift; 
      break; 
    default:
      printf ("unkonwn postfix %c", *endptr); 
      break; 
  }

  print_tm (tm_now); 
  time_t tick = mktime (tm_now); 
  printf ("tick = %ld\n", tick); 
  print_tm (tm_now); 
  return 0; 
}
