#include <unistd.h> 
#include "../apue.h" 

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf ("Usage: crypt key salt\n"); 
        return -1; 
    }

    char const* key = argv[1]; 
    char const* salt = argv[2]; 
    char *crypted = crypt (key, salt); 
    printf ("crypted string: %s\n", crypted); 
    return 0; 
}
