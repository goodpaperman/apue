#pragma once

#define STATCLASS_OK(x)         ((x) >= 0x0000 && (x) <= 0x00ff)
#define STATCLASS_INFO(x)       ((x) >= 0x0100 && (x) <= 0x01ff)
#define STATCLASS_REDIR(x)      ((x) >= 0x0200 && (x) <= 0x02ff)
#define STATCLASS_CLIERR(x)     ((x) >= 0x0400 && (x) <= 0x04ff)
#define STATCLASS_SRVERR(x)     ((x) >= 0x0500 && (x) <= 0x05ff)

#define STAT_OK             0x0000
#define STAT_OK_ATTRIGN     0x0001
#define STAT_OK_ATTRCON     0x0002

#define STAT_CLI_BADREQ     0x0400
#define STAT_CLI_FORBID     0x0401
#define STAT_CLI_NOAUTH     0x0402
#define STAT_CLI_NOPERM     0x0403
#define STAT_CLI_NOTPOS     0x0404
#define STAT_CLI_TIMOUT     0x0405
#define STAT_CLI_NOTFND     0x0406
#define STAT_CLI_OBJGONE    0x0407
#define STAT_CLI_TOOBIG     0x0408
#define STAT_CLI_TOOLNG     0x0409
#define STAT_CLI_BADFMT     0x040a
#define STAT_CLI_NOTSUP     0x040b
#define STAT_CLI_NOSCHM     0x040c
#define STAT_CLI_NOCHAR     0x040d
#define STAT_CLI_ATTRCON    0x040e
#define STAT_CLI_NOCOMP     0x040f
#define STAT_CLI_COMPERR    0x0410
#define STAT_CLI_FMTERR     0x0411
#define STAT_CLI_ACCERR     0x0412

#define STAT_SRV_INTERN     0x0500
#define STAT_SRV_NOTSUP     0x0501
#define STAT_SRV_UNAVAIL    0x0502
#define STAT_SRV_BADVER     0x0503
#define STAT_SRV_DEVERR     0x0504
#define STAT_SRV_TMPERR     0x0505
#define STAT_SRV_REJECT     0x0506
#define STAT_SRV_TOOBUSY    0x0507
#define STAT_SRV_CANCEL     0x0508
#define STAT_SRV_NOMULTI    0x0509

#define OP_PRINT_JOB        0x02
#define OP_PRINT_URI        0x03
#define OP_VALIDATE_JOB     0x04
#define OP_CREATE_JOB       0x05
#define OP_SEND_DOC         0x06
#define OP_SEND_URI         0x07
#define OP_CANCEL_JOB       0x08
#define OP_GET_JOB_ATTR     0x09
#define OP_GET_JOBS         0x0a
#define OP_GET_PRINTER_ATTR 0x0b
#define OP_HOLD_JOB         0x0c
#define OP_RELEASE_JOB      0x0d
#define OP_RESTART_JOB      0x0e
#define OP_PAUSE_PRINTER    0x10
#define OP_RESUME_PRINTER   0x11
#define OP_PURGE_JOBS       0x12

#define TAG_OPERATION_ATTR  0x01
#define TAG_JOB_ATTR        0x02
#define TAG_END_OF_ATTR     0x03
#define TAG_PRINTER_ATTR    0x04
#define TAG_UNSUPP_ATTR     0x05

#define TAG_UNSUPPORTED     0x10
#define TAG_UNKNOWN         0x12
#define TAG_NONE            0x13
#define TAG_INTEGER         0x21
#define TAG_BOOLEAN         0x22
#define TAG_ENUM            0x23
#define TAG_OCTSTR          0x30
#define TAG_DATETIME        0x31
#define TAG_RESOLUTION      0x32
#define TAG_INTRANGE        0x33
#define TAG_TEXTWLANG       0x35
#define TAG_NAMEWLANG       0x36
#define TAG_TEXTWOLANG      0x41
#define TAG_NAMEWOLANG      0x42
#define TAG_KEYWORD         0x44
#define TAG_URI             0x45
#define TAG_URISCHEME       0x46
#define TAG_CHARSET         0x47
#define TAG_NATULANG        0x48
#define TAG_MIMETYPE        0x49


struct ipp_hdr {
    int8_t major_version; 
    int8_t minor_version; 
    union { 
        int16_t op;  // OP_PRINT_JOB
        int16_t st;  // STAT_SRV_*
    } u; 
    int32_t request_id; 
    char attr_group[1];  // TAG_*
}; 

#define operation u.op
#define status u.st
