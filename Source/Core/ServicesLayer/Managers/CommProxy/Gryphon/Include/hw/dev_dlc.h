// supplied by Dearborn Group.
// we are using this file

#ifndef dev_dlc_h
#define dev_dlc_h

/* $Id: dev_dlc.h,v 1.4 2001/05/26 22:13:39 arny Exp $ */

/* dev_dlc.h - VPW J1850 cards:                     */
/* currently Motorola MC68HC58 DLC, Intersil HIP7010 BLIC & D.E. HC12.  */
/* For historical reasons 'DLC' is used in places where 'VPW' ought to  */
/* be used.                             */

/* EVENT definitions (FT_EVENT frames) */


#define GDLCRXOVER  0x41
#define GDLCCRCERR  0x42
#define GDLCINCOMPBYTE  0x43
#define GDLCBTERR   0x44
#define GDLCBREAK   0x45
#define GDLCNETFAULT    0x46
#define GDLCTXUNDER 0x47
#define GDLCTXARBLOST   0x48

#define GBLICERR    0x71
#define GBLICFTU    0x72


/* IOCTL definitions - comments indicate data size */


#define GDLCGETFOURX    0x11400001  /* 1    [01] */
#define GDLCSETFOURX    0x11400002  /* 1    [01] vsoni */
#define GDLCGETLOAD 0x11400003  /* 1    [01] */
#define GDLCSETLOAD 0x11400004  /* 1    [01] */
#define GDLCSENDBREAK   0x11400005  /* 0 */
#define GDLCABORTTX 0x11400006  /* 0 */
#define GDLCGETHDRMODE  0x11400007  /* 1 */
#define GDLCSETHDRMODE  0x11400008  /* 1 */
#define     GDLCHDRZERO 0x00
#define     GDLCHDRONE  0x01
#define     GDLCHDRTHREE    0x03
#define     GDLCHDRONEOBD   0xfe
#define     GDLCHDRGUESS    0xff
#define GDLCGETRXMODE   0x11400009  /* 1 */
#define GDLCSETRXMODE   0x1140000a  /* 1 */
#define     GDLCRXDEFAULT   0x00
#define     GDLCRXNOIFR 0x01    /* shouldnt be needed for normal use */

#define GVPWADDIFR  0x11410001  /* see below */
#define GVPWDELIFR  0x11410002  /* not implemented yet */

struct gvpwifr
    {
    unsigned short patternlen;
    unsigned short ifrlen;
    unsigned char nb;
    unsigned char reserved;
    unsigned short msglen;
    /* pattern */
    /* mask */
    /* ifr */
    };

#define GVPWCLRIFRS 0x11410003  /* 0 */


/* motorola DLC device driver queue */

#define MAXDLCMSG   11
#define MAXDLCBLOCK MAXPAYLOAD
#define DLCBUFSIZE  126
#define DLCBLOCKBUFSIZE 12      /* 12 uses up to 400k of kernel mem */

/* FT_DATA stat field definitions */

/* you should probably ignore GDLCSTAT_BLOCK and GDLCSTAT_MAYHAVEIFR,   */
/* they are primarily used internally by the device driver.     */

#define GDLCSTAT_BLOCK      0x80    /* msg was RXed as a block mode msg */
#define GDLCSTAT_MAYHAVEIFR 0x40    /* msg might have IFR attached */
#define GDLCSTAT_IFR        0x20    /* msg is IFR data */
#define GDLCSTAT_NB     0x10    /* normalisation bit was set */

#endif  //dev_dlc_h
