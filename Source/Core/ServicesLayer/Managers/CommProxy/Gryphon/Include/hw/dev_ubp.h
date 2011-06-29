// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_ubp.h,v 1.7 2002/05/28 22:11:52 stevelim Exp $ */

/* dev_ubp.h - 68HC08 based Ford UBP card */


/* EVENT definitions (FT_EVENT frames) */

#define GUBPNACK                0x41
#define GUBPTXFAILLIMIT         0x42
#define GUBPTXFAILECHO          0x43
#define GUBPCKSMERR     0x44
#define GUBPLENERR      0x45
#define GUBPTIMEOUT     0x46
#define GUBPUARTERR             0x47
#define GUBPRESET               0x48


/* IOCTL definitions - comments indicate data size */

#define GUBPGETBITRATE          0x11800001  /* 2 */
#define GUBPSETBITRATE          0x11800002  /* 2 */
#define GUBPGETINTERBYTE        0x11800003  /* 2 */
#define GUBPSETINTERBYTE        0x11800004  /* 2 */
#define GUBPGETNACKMODE         0x11800005  /* 1 */
#define GUBPSETNACKMODE         0x11800006  /* 1 */
#define GUBPGETRETRYDELAY   0x11800007  /* 1 */
#define GUBPSETRETRYDELAY   0x11800008  /* 1 */

#define GRESETHC08              0x11800009      /* 0 */
#define GTESTHC08COP            0x1180000A      /* 0 */

/* FT_DATA stat field definitions */

#define GUBPSTAT_NACK       0x01
