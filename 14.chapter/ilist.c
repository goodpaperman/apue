#include <fcntl.h> 
#include <stropts.h> 
#include <sys/conf.h> 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int fd, i, nmods; 
	struct str_list list; 
	if (argc != 2) { 
		printf ("usage: %s <pathname>\n", argv[0]); 
		exit (1); 
	}

	if ((fd = open (argv[1], O_RDONLY)) < 0) { 
		printf ("can't open %s\n", argv[1]); 
		exit (1); 
	}

	if (isastream (fd) == 0) { 
		printf ("%s is not a stream\n", argv[1]); 
		exit (1); 
	}

	if ((nmods = ioctl (fd, I_LIST, (void *)0)) < 0) { 
		printf ("I_LIST error for nmods\n"); 
		exit (1); 
	}

	printf ("%s : #%d\n", argv[1], nmods); 
	list.sl_modlist = calloc (nmods, sizeof (struct str_mlist)); 
	if (list.sl_modlist == NULL) { 
		printf ("calloc error\n"); 
		exit (1); 
	}

	list.sl_nmods = nmods; 
	if (ioctl (fd, I_LIST, &list) < 0) { 
		printf ("I_LIST error for list\n"); 
		free (list.sl_modlist); 
		exit (1); 
	}

	for (i=1; i<=nmods; ++ i) { 
		printf ("	%s:	%s\n", 
			(i == nmods) ? "driver" : "module", 
			list.sl_modlist++->l_name); 
	}

	return 0; 
}
