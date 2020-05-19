#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#if defined(BSD) || defined(MACOS)
#  include <netinet/in.h>
#endif
#include <netdb.h>
#include <errno.h>

#define CONFIG_FILE         "/etc/printer.conf"
#define SPOOLDIR            "/var/spool/printer" // base dir
#define JOBFILE             "jobno" // next print file
#define DATADIR             "data"  // printing file
#define REQDIR              "reqs"  // every print reqeust

#define FILENMSZ            64
#define FILEPERM            (S_IRUSR|S_IWUSR)  // only seen by its owner
#define USERNM_MAX          64
#define JOBNM_MAX           256
#define MSGLEN_MAX          512

#ifndef HOST_NAME_MAX
#  define HOST_NAME_MAX     256
#endif

#define IPP_PORT            631
#define PRINTSVC_PORT       874
#define QLEN                10  // listen backlog
#define IBUFSZ              512 // ipp header buffer
#define HBUFSZ              512 // http header buffer
#define IOBUFSZ             8192 // body buffer 

#ifndef ETIME
#  define ETIME ETIMEDOUT
#endif

extern int getaddrlist (const char*, const char *, struct addrinfo **); 
extern char *get_printserver (void); 
#ifdef USE_APUE_ADDRLIST
extern struct addrinfo *get_printaddr (void); 
#else
extern char *get_printname (void); 
#endif
extern ssize_t tread (int, void *, size_t, unsigned int); 
extern ssize_t treadn (int, void *, size_t, unsigned int); 
extern int connect_retry (int, const struct sockaddr *, socklen_t); 
//extern int initserver (int, struct sockaddr *, socklen_t, int); 

struct printreq {
    long size; 
    long flags; 
    char usernm [USERNM_MAX]; 
    char jobnm [JOBNM_MAX]; 
}; 

#define PR_TEXT 0x01

struct printresp {
    long retcode;  // 0: success
    long jobid; 
    char msg [MSGLEN_MAX]; 
}; 


