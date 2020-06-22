
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
#include <string.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

void get_random_name (char *name, int len, int level)
{
  int i, n; 
  sprintf (name, "%d", level); 
  n = strlen (name); 
  for (i=n; i<len; ++ i)
    name[i] = rand () % 26 + 'a'; 

  name [len] = 0; 
}

int main (int argc, char *argv[])
{
  int dir_len = 0, file_len = 0; 
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

  // add 1 (/) to 10 to be conveniently to compute how low the path is with level.
  dir_len = 9; 
  file_len = 100; 

//#if 0
//  // will fail 
//  name_len = name_max + 1; 
//#else 
//  name_len = name_max; 
//#endif 

  srand (time(0)); 
  name = (char *) calloc (1, (dir_len > file_len ? dir_len : file_len) + 1); 
  if (name == 0)
    return -1; 


  do
  {
    get_random_name (name, dir_len, ++level); 
#ifdef WIN32
    //ret = _mkdir (name); 
	  ret = CreateDirectoryA(name, NULL); 
    if (!ret)
#else 
    ret = mkdir (name, 0777); 
    if (ret == -1)
#endif 
    {
      printf ("mkdir %s failed, errno = %d, level = %d\n", name, errno, level); 
      break; 
    }
    
    printf ("mkdir %s @ %d\n", name, level); 
#ifdef WIN32
	//ret = _chdir (name); 
    ret = SetCurrentDirectoryA(name); 
    if (!ret)
#else 
    ret = chdir (name); 
    if (ret == -1)
#endif 
    {
      printf ("chdir %s failed, errno = %d, level = %d\n", name, errno, level); 
      break; 
    }

    //printf ("change to that dir\n"); 
    get_random_name (name, file_len, level); 
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
  } while (1); 

  free (name); 
  return 0; 
}
