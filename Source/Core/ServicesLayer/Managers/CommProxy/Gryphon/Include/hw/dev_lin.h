// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_lin.h,v 1.1 2002/08/09 21:27:46 stevelim Exp $ */

/* dev_ubp.h - 68HC08 based Ford UBP card */


/* EVENT definitions (FT_EVENT frames) */

#define GLINRESET               0x41
#define GLINBADID               0x42
#define GLINSYNC                0x43
#define GLINWAKEUP              0x44
#define GLINSLEEP               0x45
#define GLINWAKEUPFAILED        0x46
#define GLINNEWBITRATE          0x47


/* IOCTL definitions - comments indicate data size */

#define GLINGETBITRATE          0x11C00001  /* 2 */
#define GLINSETBITRATE          0x11C00002  /* 2 */
#define GLINGETBRKSPACE         0x11C00003      /* 1 */
#define GLINSETBRKSPACE         0x11C00004      /* 1 */
#define GLINGETBRKMARK          0x11C00005      /* 1 */
#define GLINSETBRKMARK          0x11C00006      /* 1 */
#define GLINGETIDDELAY          0x11C00007      /* 1 */
#define GLINSETIDDELAY          0x11C00008      /* 1 */
#define GLINGETRESPDELAY        0x11C00009      /* 1 */
#define GLINSETRESPDELAY        0x11C0000A      /* 1 */
#define GLINGETINTERBYTE        0x11C0000B      /* 1 */
#define GLINSETINTERBYTE        0x11C0000C      /* 1 */
#define GLINGETWAKEUPDELAY      0x11C0000D      /* 1 */
#define GLINSETWAKEUPDELAY      0x11C0000E      /* 1 */
#define GLINGETWAKEUPTIMEOUT    0x11C0000F      /* 1 */
#define GLINSETWAKEUPTIMEOUT    0x11C00010      /* 1 */
#define GLINGETWUTIMOUT3BR      0x11C00011      /* 2 */
#define GLINSETWUTIMOUT3BR      0x11C00012      /* 2 */
#define GLINSENDWAKEUP          0x11C00013      /* 0 */
#define GLINGETMODE             0x11C00014      /* 1 */
#define GLINSETMODE             0x11C00015      /* 1 */


/* MODE field definitions */

#define GLIN_MODE_AUTO          0
#define GLIN_MODE_SLAVE         1
#define GLIN_MODE_MASTER        2
#define GLIN_MODE_LOCK_SLAVE    3
#define GLIN_MODE_LOCK_MASTER   4
