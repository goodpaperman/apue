#! /usr/bin/stap

global recv, xmit
#global histogram

probe begin 
{
  printf ("Starting network capture (Ctl-C to end)\n")
}

probe netdev.receive
{
  recv[dev_name, pid (), execname ()] <<< length
  #histogram <<< length
}

probe netdev.transmit
{
  xmit[dev_name, pid (), execname ()] <<< length
  #histogram <<< length
}

probe end 
{
  printf ("\nend capture\n\n")
  printf ("Iface Process........ PID.. RcvPktCnt XmtPktCnt\n")
  foreach ([dev, pid, name] in recv) {
    recvcount = @count(recv[dev, pid, name])
    xmitcount = @count(xmit[dev, pid, name])
    printf ("%5s %-15s %-5d %9d %9d\n", dev, name, pid, recvcount, xmitcount)
  }

  printf ("histogram:\n")
  #print (@hist_log(histogram))

  delete recv
  delete xmit
}
