// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_kwp.h,v 1.2 1999/10/16 04:45:56 cvs Exp $ */

/* dev_kwp.h - DE KWP/BDLC card */

/* EVENT definitions (FT_EVENT frames) */

#define GKWPWAKEFIVE    0x41        /* five baud wake up seen */
#define GKWPWAKEFAST    0x42        /* fast wake up seen */
#define GKWPBUSIDLE 0x43        /* bus idle */

/* IOCTL definitions - comments indicate data size */

#define GKWPSETPTIMES   0x11700011  /* 16 */
#define GKWPSETWTIMES   0x11700010  /* 20 */
#define GKWPDOWAKEUP    0x11700008  /* 0 */
#define GKWPGETBITTIME  0x11700101  /* 2 */
#define GKWPSETBITTIME  0x11700102  /* 2 vsoni */
#define GKWPSETNODEADDR 0x11700104  /* 1 vsoni */
#define GKWPGETNODETYPE 0x11700105  /* 1 */
#define GKWPSETNODETYPE 0x11700106  /* 1 vsoni */
#define     GKWPMONITOR 0x00
#define     GKWPECU     0x01
#define     GKWPTESTER  0x02
#define GKWPSETWAKETYPE 0x11700108      /* 1 */
#define     GKWPFAST    0x00
#define     GKWPFIVEBAUD    0x02
#define GKWPSETTARGADDR 0x1170010a      /* 1 */
#define GKWPSETKEYBYTES 0x1170010c      /* 2 */
#define GKWPSETSTARTREQ 0x1170010e      /* 5 */
#define GKWPSETSTARTRESP    0x11700110  /* 8 */
#define GKWPSETPROTOCOL     0x11700112  /* 1 vsoni */
#define     GKWPKWP2000 0x01
#define     GKWPISO9141FORD 0x02
#define GKWPGETLASTKEYBYTES 0x11700201  /* 2 */
#define GKWPSETLASTKEYBYTES 0x11700202  /* 2 */



/* FT_DATA stat field definitions */

#define GKWPSTAT_KLINE      0x01
#define GKWPSTAT_LLINE      0x02
#define GKWPSTAT_ERR        0x04    /* general bus error */
#define GKWPSTAT_CSERR      0x08    /* checksum error */
#define GKWPSTAT_CONTENTION 0x10    /* bus contention detected */
#define GKWPSTAT_BLOCK      0x20    /* msg rx over mult interrupts */
