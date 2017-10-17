#! /bin/awk -f
{
  printf ("grep %s:\n", $0)
  system ("grep "$0" /usr/include -R")
  printf ("\n")
}
