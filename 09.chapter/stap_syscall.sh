#! /usr/bin/stap

global syscalllist
global histogram

probe begin 
{
  printf ("System Call Monitoring Started (10 seconds)...\n")
}

probe syscall.*
{
  #syscalllist[pid (), execname ()]++
  //if (execname () == "curl")
    syscalllist[probefunc ()]++

  histogram <<< pid ()
}

probe timer.ms (10000) 
{
  #foreach ([pid, procname] in syscalllist) {
  #  printf ("%s[%d] = %d\n", procname, pid, syscalllist[pid, procname])
  #}

  foreach ([func] in syscalllist) {
    printf ("%s = %d\n", func, syscalllist[func])
  }

  printf ("histogram:\n")
  print (@hist_log(histogram))
  exit ()
}

probe end 
{
  log ("end to probe")
}
