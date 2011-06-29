// supplied by Dearborn Group.
// we are using this file


/* $Id: dev_527.h,v 1.6 2002/02/28 19:34:26 stevelim Exp $ */

/* dev_527.h - 82527-based CAN and SWCAN boards */

/* EVENT definitions (FT_EVENT frames) */

#define GCANBUSOFF  0x41    /* Bus OFF */
#define GCANBUSWARN 0x42    /* Bus WARNING */
#define GCANBUSOK   0x43    /* Bus OK (return from Bus WARNING state) */
#define GCANERRFRAME    0x44    /* Error Frame / Stuff Error */
#define GCANARBLOST 0x45    /* Arbitration Lost (1 data byte- arblost pos) */
#define GCANWAKEUP  0x46    /* Wake-Up */
#define GCANCRC     0x47    /* CRC error */
#define GCANRXOVER  0x48    /* RX overflow */
#define GCANFORM    0x49    /* Form error */
#define GCANACK     0x4a    /* Ack error */
#define GCANBITONE  0x4b    /* Bit 1 error */
#define GCANBITZERO 0x4c    /* Bit 0 error */
#define GCANSWHS    0x80    /* SWCAN transition to high speed */
#define GCANSWLS    0x81    /* SWCAN transition to low speed */

#define GCAN992ERR  0xa0    /* ISO 11992 physical error */


/* Device driver queue */

#define CANBUFSIZE  1001        /* number of msgs device cant buffer */
#define MAXCANPAYLOAD   8
#define MAXCANMSG   12

/* IOCTL definitions - comments indicate data size */

#define GCANGETBTRS 0x11200001  /* 2 */
#define GCANSETBTRS 0x11200002  /* 2    value set on next init */
#define GCANGETBC   0x11200003  /* 1 */
#define GCANSETBC   0x11200004  /* 1    vsoni */
#define GCANGETMODE 0x11200005  /* 1 */
#define GCANSETMODE 0x11200006  /* 1    vsoni */
#define     GCANDATA    0x00
#define     GCANREMOTERX    0x01
#define     GCANREMOTETX    0x02
#define     GCANREMOTERESP  0x03

#define GCANGETTRANS    0x11200009  /* 1    [01] */
#define GCANSETTRANS    0x1120000a  /* 1    [01] immediate */
#define GCANSENDERR 0x1120000b  /* 0 */

#define GCANRGETOBJ 0x11210001  /* lots */
#define GCANRSETSTDID   0x11210002  /* 3 */
#define GCANRSETEXTID   0x11210003  /* 5 */
#define GCANRSETDATA    0x11210004  /* upto 10 */
#define GCANRENABLE 0x11210005  /* 1 */
#define GCANRDISABLE    0x11210006  /* 1 */
#define GCANRGETMASKS   0x11210007  /* 6 */
#define GCANRSETMASKS   0x11210008  /* 6 */

#define GCANSWGETMODE   0x11220001  /* 1 */
#define GCANSWSETMODE   0x11220002  /* 1 */
#define     GCANSWTOOLRES   0x01
#define     GCANSWSLEEP 0x02
#define     GCANSWHIVOLT    0x04
#define     GCANSWHISPEED   0x08

#define GCANSWSTAT_HIVOLT   GCANSWHIVOLT    /* only HIVOLT is in stat */

#define GCAN992GETMODE  GCANSWGETMODE   /* 1 */
#define GCAN992SETMODE  GCANSWSETMODE   /* 1 */
/* these are a guess, swap them about if they're wrong or delete these  */
/* comments if they're not:                     */
#define     GCAN992HIGH 0x01
#define     GCAN992LOW  0x02

