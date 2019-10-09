#include "../apue.h" 
#include <netdb.h>
#include <arpa/inet.h>

char * addr_type (int addrtype)
{
    switch (addrtype)
    {
        case AF_INET:
            return "AF_INET"; 
        case AF_INET6:
            return "AF_INET6"; 
        case AF_UNIX:
            return "AF_UNIX"; 
        case AF_UNSPEC:
            return "AF_UNSPEC"; 
        default:
            return "UNKNOWN"; 
    }
}

char* addr_str (int addrtype, struct sockaddr_in *addr)
{
    static char buf[128] = { 0 }; 
    inet_ntop (addrtype, addr, buf, sizeof (buf)); 
    sprintf (buf + strlen(buf), ":%d", addr->sin_port); 
    return buf; 
}

void dump_host (struct hostent *h)
{
    printf ("host entry: \n"
            "  name: %s\n"
            "  addrtype: %s\n"
            "  addrlen: %d\n"
            "  aliases: \n", 
            h->h_name, 
            addr_type(h->h_addrtype), 
            h->h_length); 

            // already host bytes order!
            //ntohl(h->h_addrtype), 
            //ntohl(h->h_length)); 


    int i = 0; 
    char **ptr = h->h_aliases; 
    while (ptr[i] != 0)
    {
        printf ("    alias[%d]: %s\n", i, ptr[i]); 
        i ++; 
    }
            
    i = 0; 
    struct sockaddr_in** addr = (struct sockaddr_in **)h->h_addr_list; 
    while (addr[i] != 0)
    {
        printf ("    addr[%d]: %s\n", i, addr_str(h->h_addrtype, addr[i])); 
        i ++; 
    }

    printf ("\n"); 
}

void test_host ()
{
    printf ("start walk around host entry\n"); 
    struct hostent* h = 0; 
    while ((h = gethostent()) != 0)
    {
        dump_host (h); 
    }

    endhostent (); 
    printf ("end\n"); 
}

void test_host2 (char const* hostname)
{
    struct hostent *h = gethostbyname (hostname); 
    if (h)
    {
        printf ("find host by name: %s\n", hostname); 
        dump_host (h); 
    }
    else 
        printf ("no host found by name: %s\n", hostname); 
}

int main (int argc, char *argv[])
{
    test_host (); 
    test_host2 ("localhost"); 
    return 0; 
}
