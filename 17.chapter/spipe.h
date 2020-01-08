#include <sys/stat.h>

int cli_conn(const char *name);
int serv_listen (const char *name);
int serv_accept (int listenfd, uid_t *uidptr);
