#! /bin/sh
( echo "Script started on " `date`; 
  ./pty "${SHELL:-/bin/bash}"; 
  echo "Script done on " `date` ) | tee typescript
