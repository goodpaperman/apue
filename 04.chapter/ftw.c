#include "../apue.h" 
#include <ftw.h> 


int ftw_func (char const* fpath, 
              struct stat const* sb, 
              int typeflag)
{
  //printf ("%s\n", fpath); 
  switch (typeflag)
  {
    case FTW_F:
      printf ("[R] %s\n", fpath); 
      break; 
    case FTW_D:
      printf ("[D] %s\n", fpath); 
      break; 
    case FTW_DNR:
      printf ("[DNR] %s\n", fpath); 
      break; 
    case FTW_NS:
      printf ("[NS] %s\n", fpath); 
      break; 
    default:
      printf ("unknown typeflag %d\n", typeflag); 
      return -1; 
  }
  return 0; 
}

int main (int argc, char *argv[])
{
  char const* dir = 0; 
  if (argc < 2)
    dir = "."; 
  else 
    dir = argv[1]; 

  int ret = ftw (dir, ftw_func, 1000); 
  return ret; 
}
