#! /usr/bin/stap
probe begin 
{
  printf ("%-8s %-16s %-5s %-16s %6s %-16s\n", "SPID", "SNAME", "RPID", "RNAME", "SIGNUM", "SIGNAME")
}

probe signal.send
{
  #if (sig_pid == target ())
  #if (execname() == @1 || pid_name == @1)
  #if (sig_name == "SIGTTOU")
    printf ("%-8d %-16s %-5d %-16s %-6d %-16s\n", pid (), execname (), sig_pid, pid_name, sig, sig_name)
}

#probe timer.ms (80000) # after 4 seconds
#{
#  exit ()
#}

probe end 
{
  log ("end to probe")
}
