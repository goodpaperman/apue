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

void test_net2 (char const* netname)
{
    struct netent *h = getnetbyname (netname); 
    if (h)
    {
        printf ("find net by name: %s\n", netname); 
        dump_net (h); 
    }
    else 
        printf ("no net found by name: %s\n", netname); 
}

void test_net3 (in_addr_t addr)
{
    struct netent *h = getnetbyaddr (addr, AF_INET); 
    if (h)
    {
        printf ("find net by addr: 0x%08x\n", addr); 
        dump_net (h); 
    }
    else 
        printf ("no net found by addr: 0x%08x\n", addr); 
}

void dump_proto (struct protoent *h)
{
    printf ("protocol entry: \n"
            "  name: %s\n"
            "  proto: %d\n"
            "  aliases: \n", 
            h->p_name, 
            h->p_proto);

    int i = 0; 
    char **ptr = h->p_aliases; 
    while (ptr[i] != 0)
    {
        printf ("    alias[%d]: %s\n", i, ptr[i]); 
        i ++; 
    }
            
    printf ("\n"); 
}

void test_proto ()
{
    printf ("start walk around protocol entry\n"); 
    struct protoent* h = 0; 
    while ((h = getprotoent()) != 0)
    {
        dump_proto (h); 
    }

    endprotoent (); 
    printf ("end\n"); 
}

void test_proto2 (char const* protoname)
{
    struct protoent *h = getprotobyname (protoname); 
    if (h)
    {
        printf ("find protocol by name: %s\n", protoname); 
        dump_proto (h); 
    }
    else 
        printf ("no protocol found by name: %s\n", protoname); 
}

void test_proto3 (int num)
{
    struct protoent *h = getprotobynumber (num); 
    if (h)
    {
        printf ("find protocol by number: %d\n", num); 
        dump_proto (h); 
    }
    else 
        printf ("no protocol found by number: %d\n", num); 
}

void dump_serv (struct servent *h)
{
    printf ("server entry: \n"
            "  name: %s\n"
            "  port: %d\n"
            "  protocol: %s\n"
            "  aliases: \n", 
            h->s_name, 
            h->s_port, 
            h->s_proto);

    int i = 0; 
    char **ptr = h->s_aliases; 
    while (ptr[i] != 0)
    {
        printf ("    alias[%d]: %s\n", i, ptr[i]); 
        i ++; 
    }
            
    printf ("\n"); 
}

void test_serv ()
{
    printf ("start walk around server entry\n"); 
    struct servent* h = 0; 
    while ((h = getservent()) != 0)
    {
        dump_serv (h); 
    }

    endservent (); 
    printf ("end\n"); 
}

void test_serv2 (char const* servname)
{
    struct servent *h = getservbyname (servname, "tcp"); 
    if (h == NULL)
        h = getservbyname (servname, "udp"); 

    if (h)
    {
        printf ("find server by name: %s\n", servname); 
        dump_serv (h); 
    }
    else 
        printf ("no server found by name: %s\n", servname); 
}

void test_serv3 (int port)
{
    struct servent *h = getservbyport (port, "tcp"); 
    if (h == NULL)
        h = getservbyport (port, "udp"); 

    if (h)
    {
        printf ("find server by port: %d\n", port); 
        dump_serv (h); 
    }
    else 
        printf ("no server found by port: %d\n", port); 
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
    test_net2 ("default"); 
    test_net2 ("loopback"); 
    test_net2 ("eth0"); 
    //test_net3 (0x0000007f); 
    test_net3 (0x7f000000); 
    //test_net3 (0xdf38810a); 
    test_net3 (0x0a8138df); 

    test_proto (); 
    test_proto2 ("tcp"); 
    test_proto2 ("raw"); 
    test_proto3 (136); 
    test_proto3 (224); 

    test_serv (); 
    test_serv2 ("http"); 
    test_serv2 ("abcd"); 
    test_serv3 (80); 
    test_serv3 (8080); 

    return 0; 
}
