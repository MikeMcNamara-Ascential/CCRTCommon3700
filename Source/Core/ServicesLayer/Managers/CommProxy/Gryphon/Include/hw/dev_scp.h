// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_scp.h,v 1.3 2002/06/03 22:00:00 jason Exp $ */

/* dev_scp.h - HBCC-based Ford J1850 board */

/* EVENT definitions (FT_EVENT frames) */



#define GSCPRXOVER  0x41    /* RX overflow */
#define GSCPRXERR   0x42    /* RX Error */
#define GSCPUNACKABLE   0x43    /* UnACK-able error */
#define GSCPTXERRLIM    0x44    /* Reached transmit error limit */
#define GSCPARBLOSTLIM  0x45    /* Reached arbitration loss limit */
#define GSCPNETFAULT    0x46    /* Net fault */
#define GSCPWAKEUP  0x47    /* Wake-up */


/* Device driver queue */

#define MAXSCPMSG   12
#define SCPBUFSIZE  251

/* IOCTL definitions - comments indicate data size */

#define GSCPGETBBR  0x11300001  /* 1    [0123] */
#define GSCPSETBBR  0x11300002  /* 1    [0123] vsoni */
#define GSCPGETID   0x11300003  /* 1 */
#define GSCPSETID   0x11300004  /* 1 */
#define GSCPADDFUNCID   0x11300005  /* 1 */
#define GSCPCLRFUNCID   0x11300006  /* 0 */
#define GSCPADDFUNCRID  0x11300007  /* 1 */
#define GSCPCLRFUNCRID  0x11300008  /* 0 */
#define GSCPGETFRDATA   0x11300009  /* 1 */
#define GSCPSETFRDATA   0x1130000A  /* 2 */
    
