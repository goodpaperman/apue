#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <sys/stat.h> 

void get_random_name (char *name, int len)
{
  int i; 
  for (i=0; i<len; ++ i)
    name[i] = rand () % 26 + 'a'; 

  name [len] = 0; 
}

int main (int argc, char *argv[])
{
  //char const* path = "."; 
  char const* path = "/"; 
  int name_max = pathconf (path, _PC_NAME_MAX); 
  int path_max = pathconf (path, _PC_PATH_MAX); 
  printf ("NAME_MAX = %d, PATH_MAX = %d\n", 
    name_max, path_max); 

  name_max = 25; 
  int name_len = 0; 

#if 0
  // will fail 
  name_len = name_max + 1; 
#else 
  name_len = name_max; 
#endif 
  char *name = (char *) malloc (name_len + 1); 
  if (name == 0)
    return -1; 

  get_random_name (name, name_len); 
  int fd = open (name, O_RDWR | O_CREAT, 0644); 
  if (fd == -1)
    printf ("open %s failed, errno = %d\n", name, errno); 
  else 
  {
    printf ("open %s OK.\n", name); 
    close (fd); 
  }

  int ret = 0, level = 0; 
  do
  {
    level ++; 
    get_random_name (name, name_len); 
    ret = mkdir (name, 0777); 
    if (ret == -1)
    {
      printf ("mkdir %s failed, errno = %d, level = %d\n", name, errno, level); 
      break; 
    }

    ret = chdir (name); 
    if (ret == -1)
    {
      printf ("chdir %s failed, errno = %d, level = %d\n", name, errno, level); 
      break; 
    }

    printf ("mkdir %s @ %d\n", name, level); 
  } while (1); 

  free (name); 
  return 0; 
}
