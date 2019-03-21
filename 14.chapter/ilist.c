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
	if (argc > 1) { 
		if ((fd = open (argv[1], O_RDONLY)) < 0) { 
			fprintf (stderr,"can't open %s\n", argv[1]); 
			exit (1); 
		}
	}
	else 
		// use pipe as input
		fd = STDIN_FILENO; 

	if (isastream (fd) == 0) { 
		fprintf (stderr,"%s is not a stream\n", argc > 1 ? argv[1] : "stdin"); 
		exit (1); 
	}

	fprintf (stderr, "a stream\n"); 
	if ((nmods = ioctl (fd, I_LIST, (void *)0)) < 0) { 
		fprintf (stderr,"I_LIST error for nmods\n"); 
		exit (1); 
	}

	fprintf (stderr,"%s : #%d\n", argc > 1 ? argv[1] : "stdin", nmods); 
	list.sl_modlist = calloc (nmods, sizeof (struct str_mlist)); 
	if (list.sl_modlist == NULL) { 
		fprintf (stderr,"calloc error\n"); 
		exit (1); 
	}

	list.sl_nmods = nmods; 
	if (ioctl (fd, I_LIST, &list) < 0) { 
		fprintf (stderr,"I_LIST error for list\n"); 
		free (list.sl_modlist); 
		exit (1); 
	}

	for (i=1; i<=nmods; ++ i) { 
		fprintf (stderr,"	%s:	%s\n", 
			(i == nmods) ? "driver" : "module", 
			list.sl_modlist++->l_name); 
	}

	return 0; 
}
