
int tty_cbreak (int fd);
int tty_raw (int fd);
int tty_reset (int fd);
void tty_atexit (void);
struct termios* tty_termios (void);
