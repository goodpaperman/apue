#include "../apue.h" 
#include <sys/types.h> 
#include <pwd.h> 


int main (int argc, char *argv[]) 
{
    struct passwd *pwd=0; 
    char const* login = getlogin (); 
    if (login == NULL)
        err_sys("getlogin"); 

    printf ("login name: %s\n", login); 
    pwd = getpwnam (login); 
    printf ("login user id: %d\n", pwd->pw_uid); 
    printf ("login user shell: %s\n", pwd->pw_shell); 

    pwd = getpwuid (getuid ()); 
    printf ("real user name: %s\n", pwd->pw_name); 

    pwd = getpwuid (geteuid ()); 
    printf ("effect user name: %s\n", pwd->pw_name); 
    return 0; 
}
