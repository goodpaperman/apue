#include "../apue.h" 

enum
{
    ef_free,  /* `ef_free' MUST be zero!  */
    ef_us,
    ef_on,
    ef_at,
    ef_cxa
};

struct exit_function
{
    /* `flavour' should be of type of the `enum' above but since we need
     *        this element in an atomic operation we have to use `long int'.  */
    long int flavor;
    union
    {
        void (*at) (void);
        struct
        {
            void (*fn) (int status, void *arg);
            void *arg;
        } on;
        struct
        {
            void (*fn) (void *arg, int status);
            void *arg;
            void *dso_handle;
        } cxa;
    } func;
};

struct exit_function_list
{
    struct exit_function_list *next;
    size_t idx;
    struct exit_function fns[32];
};

extern struct exit_function_list *__exit_funcs; 

void dump_exit_func (struct exit_function *ef)
{
    switch (ef->flavor)
    {
        case ef_free:
            printf ("free slot\n"); 
            break; 
        case ef_us:
            printf ("occupy slot\n"); 
            break; 
        case ef_on: 
            printf ("on_exit function: %p, arg: %p\n", ef->func.on.fn, ef->func.on.arg); 
            break; 
        case ef_at:
            printf ("atexit function: %p\n", ef->func.at); 
            break; 
        case ef_cxa:
            printf ("cxa_exit function: %p, arg: %p, dso: %p\n", ef->func.cxa.fn, ef->func.cxa.arg, ef->func.cxa.dso_handle); 
            break; 
        default:
            printf ("unknown type: %d\n", ef->flavor); 
            break; 
    }
}

void dump_exit ()
{
    struct exit_function_list *l = __exit_funcs; 
    while (l != NULL)
    {
        printf ("total %d record\n", l->idx); 
        for (int i=0; i<l->idx; ++ i)
        {
            dump_exit_func (&l->fns[i]); 
        }

        l = l->next; 
    }
}


void do_dirty_work ()
{
    printf ("doing dirty works!\n"); 
}

void bye ()
{
    printf ("bye, forks~\n"); 
}

void times ()
{
    static int counter = 32; 
    printf ("times %d\n", counter--); 
}

int main ()
{
  int ret = 0; 
  printf ("__exit_funcs: %p\n", __exit_funcs); 
  ret = atexit (do_dirty_work); 
  if (ret != 0)
      err_sys ("atexit"); 
  else 
      printf ("register do_dirty_work %p\n", (void *)do_dirty_work); 

  ret = atexit (bye); 
  if (ret != 0)
      err_sys ("bye1"); 
  else 
      printf ("register bye %p\n", (void *)bye); 

  ret = atexit (times); 
  if (ret != 0)
      err_sys ("times"); 
  else 
      printf ("register times %p\n", (void *)times); 

  dump_exit (); 
  printf ("main is done!\n"); 
  return 0; 
}
