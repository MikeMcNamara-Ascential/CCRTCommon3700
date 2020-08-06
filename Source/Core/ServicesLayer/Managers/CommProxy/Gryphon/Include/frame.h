// supplied by Dearborn Group.
// we are using this file

#ifndef frame_h
#define frame_h

/* $Id: frame.h,v 1.1 1999/10/04 21:40:29 jason Exp $ */
/* Gryphon communication protocol structures */

// these structures are used in the Msg structure in gmsg.h                
struct gframehdr
    {
    unsigned char src;
    unsigned char srcchan;
    unsigned char dst;
    unsigned char dstchan;

    unsigned short msglen;
    unsigned char frametype;
    unsigned char reserved;
    };

struct gdatahdr
    {
    unsigned char hdrlen;
    unsigned char hdrbits;
    unsigned short datalen;

    unsigned char extralen;
    unsigned char mode;
    unsigned char priority;
    unsigned char stat;

    unsigned long time;

    unsigned char context;
    unsigned char reservedchar;
    unsigned short reservedshort;
    };

struct gcmdhdr
    {
    unsigned char cmd;
    unsigned char context;
    unsigned short reserved;
    };

struct gresphdr
    {
    struct gcmdhdr cmdhdr;
    unsigned long resp;
    };

struct geventhdr
    {
    unsigned char event;
    unsigned char context;
    unsigned short reserved;
    unsigned long time;
    };

#endif  //frame_h
