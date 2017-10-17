#include "../apue.h" 
#include <setjmp.h>
#include <limits.h> 

#define TOK_ADD 1
#define TOK_SUB 2
#define TOK_MUL 3
#define TOK_DIV 4

int total = 0; 
jmp_buf jmpbuff; 
char *tok_ptr; 

int get_token ()
{
  char *end = 0; 
  long ret = strtol (tok_ptr, &end, 10); 
  if (ret == 0 && tok_ptr == end)
    return INT_MIN; 

  tok_ptr = end; 
  return ret; 
}

void cmd_div ()
{
  int token = 0; 
  token = get_token (); 
  if (token == INT_MIN)
    longjmp (jmpbuff, TOK_DIV); 

  if (token == 0)
    longjmp (jmpbuff, 5 /*divide by zero*/); 

  printf ("%d / %d = %d\n", total, token, total / token); 
  total /= token; 
}

void cmd_mul ()
{
  int token = 0; 
  token = get_token (); 
  if (token == INT_MIN)
    longjmp (jmpbuff, TOK_MUL); 

  printf ("%d * %d = %d\n", total, token, total * token); 
  total *= token; 
}

void cmd_sub ()
{
  int token = 0; 
  token = get_token (); 
  if (token == INT_MIN)
    longjmp (jmpbuff, TOK_SUB); 

  printf ("%d - %d = %d\n", total, token, total - token); 
  total -= token; 
}

void cmd_add ()
{
  int token = 0; 
  token = get_token (); 
  if (token == INT_MIN)
    longjmp (jmpbuff, TOK_ADD); 

  printf ("%d + %d = %d\n", total, token, total + token); 
  total += token; 
}

void do_line (char *line)
{
  int cmd = 0; 
  tok_ptr = line; 
  while ((cmd = get_token ()) != INT_MIN)
  {
    switch (cmd)
    {
      case TOK_ADD:
        cmd_add (); 
        break; 
      case TOK_SUB:
        cmd_sub (); 
        break; 
      case TOK_MUL:
        cmd_mul (); 
        break; 
      case TOK_DIV:
        cmd_div (); 
        break; 
      default:
        printf ("unknown cmd %d\n", cmd); 
        longjmp (jmpbuff, 0); 
        break; 
    }
  }
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  char line [MAXLINE] = { 0 }; 
  if ((ret = setjmp (jmpbuff)) != 0)
    //err_ret ("setjmp"); 
    printf ("setjmp %d\n", ret); 

  while (fgets (line, MAXLINE, stdin) != NULL)
    do_line (line); 

  return 0; 
}

