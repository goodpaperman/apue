#! /bin/awk -f
BEGIN { 
  for (i =0; i<ARGC; i++)
    printf "argv[%d]: %s\n", i, ARGV[i]
  exit 
}

