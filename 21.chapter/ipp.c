#include "ipp.h"

struct err_to_msg
{
    int err; 
    char const* msg; 
}; 

#define DEF_ERR_MSG(x) { x, #x }, 

struct err_to_msg etm[] = 
{
    DEF_ERR_MSG(STAT_CLI_BADREQ    ) 
    DEF_ERR_MSG(STAT_CLI_FORBID    ) 
    DEF_ERR_MSG(STAT_CLI_NOAUTH    ) 
    DEF_ERR_MSG(STAT_CLI_NOPERM    ) 
    DEF_ERR_MSG(STAT_CLI_NOTPOS    ) 
    DEF_ERR_MSG(STAT_CLI_TIMOUT    ) 
    DEF_ERR_MSG(STAT_CLI_NOTFND    ) 
    DEF_ERR_MSG(STAT_CLI_OBJGONE   ) 
    DEF_ERR_MSG(STAT_CLI_TOOBIG    ) 
    DEF_ERR_MSG(STAT_CLI_TOOLNG    ) 
    DEF_ERR_MSG(STAT_CLI_BADFMT    ) 
    DEF_ERR_MSG(STAT_CLI_NOTSUP    ) 
    DEF_ERR_MSG(STAT_CLI_NOSCHM    ) 
    DEF_ERR_MSG(STAT_CLI_NOCHAR    ) 
    DEF_ERR_MSG(STAT_CLI_ATTRCON   ) 
    DEF_ERR_MSG(STAT_CLI_NOCOMP    ) 
    DEF_ERR_MSG(STAT_CLI_COMPERR   ) 
    DEF_ERR_MSG(STAT_CLI_FMTERR    ) 
    DEF_ERR_MSG(STAT_CLI_ACCERR    ) 
    DEF_ERR_MSG(STAT_SRV_INTERN    ) 
    DEF_ERR_MSG(STAT_SRV_NOTSUP    ) 
    DEF_ERR_MSG(STAT_SRV_UNAVAIL   ) 
    DEF_ERR_MSG(STAT_SRV_BADVER    ) 
    DEF_ERR_MSG(STAT_SRV_DEVERR    ) 
    DEF_ERR_MSG(STAT_SRV_TMPERR    ) 
    DEF_ERR_MSG(STAT_SRV_REJECT    ) 
    DEF_ERR_MSG(STAT_SRV_TOOBUSY   ) 
    DEF_ERR_MSG(STAT_SRV_CANCEL    ) 
    DEF_ERR_MSG(STAT_SRV_NOMULTI   ) 
}; 

char const* ipp_err_to_msg (int err)
{
    int i; 
    for (i=0; i<sizeof (etm) / sizeof (struct err_to_msg); ++ i)
        if (etm[i].err == err)
            return etm[i].msg; 

    return 0; 
}
