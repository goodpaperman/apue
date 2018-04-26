#ifndef APUE_LOG_H
#define APUE_LOG_H

void openLog (char const* filename); 
void writeLog(const char *fmt, ...); 
void closeLog (); 

#endif 
