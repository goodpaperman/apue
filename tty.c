#include "apue.h"
#include <termios.h>
#include <errno.h>

#define USE_OPOST

static struct termios save_termios; 
static int ttysavefd = -1; 
static enum { RESET, RAW, CBREAK } ttystate = RESET; 

#ifdef PRINT_FLAG
extern void print_local_flag (tcflag_t flag);
extern void print_input_flag (tcflag_t flag); 
extern void print_control_flag (tcflag_t flag); 
extern void print_output_flag (tcflag_t flag); 
#endif

int tty_cbreak (int fd)
{
  int err; 
  struct termios buf; 

  if (ttystate != RESET) {
    errno = EINVAL; 
    return -1; 
  }

  if (tcgetattr (fd, &buf) < 0)
    return -1; 

  save_termios = buf; 
  buf.c_lflag &= ~(ECHO | ICANON); 
  buf.c_cc[VMIN] = 1; 
  buf.c_cc[VTIME] = 0; 

  if (tcsetattr (fd, TCSAFLUSH, &buf) < 0)
    return -1; 

  if (tcgetattr (fd, &buf) < 0) {
    err = errno; 
    tcsetattr (fd, TCSAFLUSH, &save_termios); 
    errno = err; 
    return -1; 
  }

  if ((buf.c_lflag & (ECHO | ICANON)) ||
      buf.c_cc[VMIN] != 1 ||
      buf.c_cc[VTIME] != 0) {
    tcsetattr (fd, TCSAFLUSH, &save_termios); 
    errno = EINVAL; 
    return -1; 
  }

  ttystate = CBREAK; 
  ttysavefd = fd; 
  return 0; 
}


int tty_raw (int fd)
{
  int err; 
  struct termios buf; 

  if (ttystate != RESET) {
    fprintf (stderr, "ttystate error\n"); 
    errno = EINVAL; 
    return -1; 
  }

  if (tcgetattr (fd, &buf) < 0) {
    fprintf (stderr, "fist tcgetattr error\n"); 
    return -1; 
  }

  save_termios = buf; 
  buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
  buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); 
  buf.c_cflag &= ~(CSIZE | PARENB); 
  buf.c_cflag |= CS8; 
#ifdef USE_OPOST
  buf.c_oflag &= ~(OPOST); 
#endif
  buf.c_cc[VMIN] = 1; 
  buf.c_cc[VTIME] = 0; 

  if (tcsetattr (fd, TCSAFLUSH, &buf) < 0) { 
    fprintf (stderr, "tcsetattr error\n"); 
    return -1; 
  }

  if (tcgetattr (fd, &buf) < 0) {
    err = errno; 
    fprintf (stderr, "second tcgetattr error\n"); 
    tcsetattr (fd, TCSAFLUSH, &save_termios); 
    errno = err; 
    return -1; 
  }

  if ((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) ||
      (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) ||
      ((buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8) || 
#ifdef USE_OPOST
      (buf.c_oflag & OPOST) ||
#endif
      buf.c_cc[VMIN] != 1 ||
      buf.c_cc[VTIME] != 0) {
    fprintf (stderr, "some fileds set failed: 0x%x 0x%x 0x%x 0x%x cc[0]=%d cc[1]=%d\n", 
        buf.c_lflag, 
        buf.c_iflag, 
        buf.c_cflag, 
        buf.c_oflag, 
        buf.c_cc[VMIN], 
        buf.c_cc[VTIME]); 

#ifdef PRINT_FLAG
    print_local_flag (buf.c_lflag); 
    print_input_flag (buf.c_iflag); 
    print_control_flag (buf.c_cflag); 
    print_output_flag (buf.c_oflag); 
#endif

    tcsetattr (fd, TCSAFLUSH, &save_termios); 
    errno = EINVAL; 
    return -1; 
  }

  ttystate = RAW; 
  ttysavefd = fd; 
  return 0; 
}

int tty_reset (int fd)
{
  if (ttystate == RESET)
    return 0; 
  if (tcsetattr (fd, TCSAFLUSH, &save_termios) < 0)
    return -1; 
  
  ttystate = RESET; 
  return 0; 
}

void tty_atexit (void)
{
  if (ttysavefd >= 0)
    tty_reset (ttysavefd); 
}

struct termios* tty_termios (void)
{
  return (&save_termios); 
}
