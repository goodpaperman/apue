#include <sys/types.h> 
//#include <sys/acct.h> 
#include <linux/acct.h> 
#include "../apue.h" 

#define ACCTFILE "/var/account/pacct"
static unsigned long compt2ulong (comp_t comptime)
{
    unsigned long val; 
    int           exp; 	
#if 1
    val = comptime & 017777; 
    exp = (comptime >> 13) & 7; 
    while (exp-- > 0)
        val *= 8; 
#else 
    val = comptime & 0x1fff; 
    exp = ((comptime >> 13) & 0x7) * 3; 
    val = val << exp; 
#endif 

    return (val); 
} 

int main (void) 
{
    int tck = sysconf (_SC_CLK_TCK); 
    printf ("clock tick: %d\n", tck); 

    struct acct_v3 acdata; 
    printf ("sizeof (acct) = %d\n", sizeof (acdata)); 

    FILE        *fp; 
    if ((fp = fopen (ACCTFILE, "r")) == NULL)
        err_sys ("can't open %s", ACCTFILE); 

    while (fread (&acdata, sizeof (acdata), 1, fp) == 1) 
    {
        //printf ("%-*.*s e = %6ld, chars = %7ld, "
        printf (//"%-10.10s e = %6ld, chars = %7ld, "
                "%-10.10s e = %-6.2f, chars = %7ld, "
                "stat = %3u: %c %c %c %c\n", 
                //sizeof (acdata.ac_comm), 
                //sizeof (acdata.ac_comm), 
                acdata.ac_comm, 
                //compt2ulong (acdata.ac_etime), 
                acdata.ac_etime, 
                compt2ulong (acdata.ac_io), 
                (unsigned char) acdata.ac_exitcode, 
#ifdef ACORE
                acdata.ac_flag & ACORE ? 'D' : ' ', 
#else 
                ' ', 
#endif 
#ifdef AXSIG
                acdata.ac_flag & AXSIG ? 'X' : ' ', 
#else 
                ' ', 
#endif 
                acdata.ac_flag & AFORK ? 'F' : ' ', 
                acdata.ac_flag & ASU   ? 'S' : ' '); 
    }

    if (ferror (fp))
        err_sys ("read error"); 

    exit (0); 
}
