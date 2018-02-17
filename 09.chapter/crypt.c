#include <unistd.h> 
#include "../apue.h" 

int main (int argc, char *argv[])
{
#if 0
    if (argc < 3)
    {
        printf ("Usage: crypt key salt\n"); 
        return -1; 
    }

    char const* key = argv[1]; 
    char const* salt = argv[2]; 
#else 
    char buf[128] = { 0 }; 
    char const* key = fgets (buf, sizeof(buf), stdin); 
    char const* salt = getpass ("enter password:"); 
#endif 
    char *crypted = crypt (key, salt); 
    printf ("crypted string [%s] [%s]: %s\n", key, salt, crypted); 
    return 0; 
}
