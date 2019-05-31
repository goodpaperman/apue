#include <stdio.h> 
//#include <stropts.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h> 
#include <fcntl.h> 
#include <sys/uio.h>

void test_writev (int fd, char const* buf1, int len1, char const* buf2, int len2)
{
	struct iovec block[2]; 
	block[0].iov_base = buf1; 
	block[0].iov_len = len1; 
	block[1].iov_base = buf2; 
	block[1].iov_len = len2; 
	int ret = writev (fd, block, 2); 
	//printf ("total write %d\n", ret); 
	if (ret == -1) { 
		printf ("fatal error while writev, errno %d\n", errno); 
		exit(-1); 
	}
}

void test_write (int fd, char const* buf1, int len1, char const* buf2, int len2)
{
    int len = len1 + len2; 
    char *buf = (char *)malloc (len); 
    memcpy (buf, buf1, len1); 
    memcpy (buf+len1, buf2, len2); 

    int ret = write (fd, buf, len); 
    free (buf); 
	//printf ("total write %d\n", ret); 
	if (ret == -1) { 
		printf ("fatal error while write, errno %d\n", errno); 
		exit(-1); 
	}
}

int main (int argc, char *argv[])
{
	if (argc < 4) { 
		printf ("Usage: linu_writev file times mode\n"); 
		exit (0); 
	}

	int fd, n; 
	fd = open (argv[1], O_WRONLY | O_TRUNC | O_CREAT); 
	if (fd == -1) { 
		printf ("open %s failed, errno %d\n", argv[1], errno); 
		exit (0); 
	}

    int total = atoi (argv[2]); 
    int mode = atoi (argv[3]); 
	char buf1[100] = { 0 }; 
	char buf2[200] = { 0 }; 
	for (n=0; n<total; ++n) { 
        if (mode)
		    test_writev(fd, buf1, sizeof(buf1), buf2, sizeof(buf2)); 
        else 
            test_write (fd, buf1, sizeof(buf1), buf2, sizeof(buf2)); 
	}

	close (fd); 
	printf ("test over, %s %d K total\n", mode ? "writev" : "write", total * 300 / 1024); 
	return 0; 
}

