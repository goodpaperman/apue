#include "../apue.h" 

int main ()
{
    int i = 1; 
    double j = 0.0; 
    float k = i / j; // to see SIG_FPE
    printf ("k = %.2f\n", k); 
    k = i / 0; 
    printf ("k = %.2f\n", k); 
    return 0; 
}
