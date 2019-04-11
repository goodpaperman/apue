#include "../apue.h" 

#define BUFFLEN 30

int main (int argc, char *argv[])
{
    char buf[BUFFLEN+1] = { 0 }; 
    size_t len = BUFFLEN; 
    while (1) {
        ssize_t n = readn (STDIN_FILENO, buf, len); 
        if (n <= 0) {
            err_sys ("readn failed, n = %d", n); 
            break; 
        }

        ssize_t m = writen (STDOUT_FILENO, buf, n); 
        if (m != n) {
            err_sys ("writen failed"); 
            break; 
        }
    }

    return 0; 
}
