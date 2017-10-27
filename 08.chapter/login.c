#include "../apue.h"

int main (int argc, char *argv[])
{
  char passwd[] = "password"; 
  if (argc < 2) { 
    printf ("usage: %s <password>\n", argv[0]); 
    return -1; 
  }

  if (!strcmp (passwd, argv[1])) {
    printf ("CORRECT!\n"); 
    return 0; 
  }

  printf ("INCORRECT!\n"); 
  return -1; 
}
