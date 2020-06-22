
#ifdef WIN32
#  ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#  define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#  endif
#include <windows.h> 
#include <direct.h> 
#else 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <sys/stat.h> 
#endif 
#include <stdio.h> 
#include <stdlib.h> 

void get_random_name (char *name, int len)
{
  int i; 
  for (i=0; i<len; ++ i)
    name[i] = rand () % 26 + 'a'; 

  name [len] = 0; 
}

int main (int argc, char *argv[])
{
  int name_len = 0; 
  char *name = 0; 
  int ret = 0, level = 0; 
#ifdef WIN32
	int name_max = MAX_PATH; 
	int path_max = MAX_PATH; 
  HANDLE fd = 0; 
#else 
  //char const* path = "."; 
  char const* path = "/"; 
  int name_max = pathconf (path, _PC_NAME_MAX); 
  int path_max = pathconf (path, _PC_PATH_MAX); 
#endif 

  printf ("NAME_MAX = %d, PATH_MAX = %d\n", 
    name_max, path_max); 

  name_max = 25; 

#if 0
  // will fail 
  name_len = name_max + 1; 
#else 
  name_len = name_max; 
#endif 

  name = (char *) malloc (name_len + 1); 
  if (name == 0)
    return -1; 

  get_random_name (name, name_len); 
#ifdef WIN32
  fd = CreateFile (name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, 0); 
  if (fd == 0)
	  printf ("open %s failed, errno = %d\n", name, errno); 
  else 
  {
	  printf ("open %s OK.\n", name); 
	  CloseHandle (fd); 
  }
#else 
  int fd = open (name, O_RDWR | O_CREAT, 0644); 
  if (fd == -1)
    printf ("open %s failed, errno = %d\n", name, errno); 
  else 
  {
    printf ("open %s OK.\n", name); 
    close (fd); 
  }
#endif 

  do
  {
    level ++; 
    get_random_name (name, name_len); 
#ifdef WIN32
	  ret = _mkdir (name); 
#else 
    ret = mkdir (name, 0777); 
#endif 
    if (ret == -1)
    {
      printf ("mkdir %s failed, errno = %d, level = %d\n", name, errno, level); 
      break; 
    }

#ifdef WIN32
	ret = _chdir (name); 
#else 
    ret = chdir (name); 
#endif 
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
