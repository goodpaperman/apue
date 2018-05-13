#! /usr/bin/stap
probe begin 
{
  log("hello systemtap!")
  exit ()
}
