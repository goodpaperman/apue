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

char* addr_str (int addrtype, struct in_addr *addr)
{
    static char buf[128] = { 0 }; 
    inet_ntop (addrtype, addr, buf, sizeof (buf)); 
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
    struct in_addr** addr = (struct in_addr **)h->h_addr_list; 
    while (addr[i] != 0)
    {
        //char *ptr = (char *) &addr[i]->sin_addr.s_addr; 
        //printf ("        0x%08x\n", addr[i]->s_addr); 
        //printf ("        %02x0%2x%02x%02x\n", ptr[0], ptr[1], ptr[2], ptr[3]); 
        printf ("    addr[%d]: %s 0x%08x\n", i, addr_str(h->h_addrtype, addr[i]), addr[i]->s_addr); 
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

void test_host3 (struct in_addr *addr)
{
    struct hostent *h = gethostbyaddr (addr, sizeof (struct in_addr), AF_INET); 
    if (h)
    {
        printf ("find host by addr: 0x%08x\n", addr->s_addr); 
        dump_host (h); 
    }
    else 
        printf ("no host found by addr: 0x%08x\n", addr->s_addr); 
}

void dump_net (struct netent *h)
{
    struct in_addr addr; 
    addr.s_addr = htonl(h->n_net); 
    printf ("net entry: \n"
            "  name: %s\n"
            "  addrtype: %s\n"
            "  net: %s 0x%08x\n"
            "  aliases: \n", 
            h->n_name, 
            addr_type(h->n_addrtype), 
            addr_str (h->n_addrtype, &addr), 
            h->n_net); 

    int i = 0; 
    char **ptr = h->n_aliases; 
    while (ptr[i] != 0)
    {
        printf ("    alias[%d]: %s\n", i, ptr[i]); 
        i ++; 
    }
            
    printf ("\n"); 
}

void test_net ()
{
    printf ("start walk around net entry\n"); 
    struct netent* h = 0; 
    while ((h = getnetent()) != 0)
    {
        dump_net (h); 
    }

    endnetent (); 
    printf ("end\n"); 
}

int main (int argc, char *argv[])
{
    test_host (); 
    test_host2 ("localhost"); 
    test_host2 ("www.baidu.com"); 
    test_host2 ("gux.glodon.com"); 
    struct in_addr addr; 
    addr.s_addr = 0x0100007f; 
    test_host3 (&addr); 
    addr.s_addr = 0xdf38810a;  // 10.129.56.223: gux.glodon.com
    //addr.s_addr = 0x0a8138df;  
    test_host3 (&addr); 
    test_net (); 
    return 0; 
}
