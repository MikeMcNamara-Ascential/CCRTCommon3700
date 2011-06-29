// supplied by Dearborn Group.
// we are using this file

#define MAXUARTMSG  64
#define UARTBUFSIZE 101

#define GUARTGETDIV     0x11a00001  /* 4 */
#define GUARTSETDIV     0x11a00002  /* 4 vsoni */
#define GUARTGETBAUDBASE    0x11a00003  /* 4 */
#define GUARTGETRXINTERMSG  0x11a00005  /* 4 */
#define GUARTSETRXINTERMSG  0x11a00006  /* 4 */

#define GUARTGETMCR     0x11a00007  /* 1 */
#define GUARTSETMCR     0x11a00008  /* 1 */
#define     GUARTDTR        0x01
#define     GUARTRTS        0x02
#define     GUARTOUT1       0x04
#define     GUARTOUT2       0x08    /* warning - see below */
#define     GUARTLOOP       0x10    /* warning - see below */

/* 162550 uses the OUT2 bit in the modem control register as an extra   */
/* interrupt enable even though it claims to be 16550 compatible.  As a */
/* work-around, the driver always sets the OUT2 bit on initialisation.  */
/* Also note that you should always OR in 0x08 with the value you call  */
/* GUARTSETMCR with, unless you know you've a real OUT2 output on your  */
/* card and you need to clear it, or you know that your client software */
/* will never ever be used with 162550 based hardware or any other chip */
/* that may act similarly (there may well be others).  Refer to 16c2250 */
/* datasheet.  Similarly, although this applies to all 16550 type   */
/* UARTs, make sure you don't set the GUARTLOOP bit unless you really   */
/* know what it does, and you really wanted to do it.           */

#define GUARTGETRXMODE      0x11a00009  /* 1 */
#define GUARTSETRXMODE      0x11a0000a  /* 1 vsoni */
#define GUARTGETTXMODE      0x11a0000b  /* 1 */
#define GUARTSETTXMODE      0x11a0000c  /* 1 vsoni */
#define     GUARTMODETIME       0x01
#define     GUARTMODECHRYSLERMSCI   0x02


/***************************************************/
#ifdef FULLYDEVELOPEDUARTDRIVER

#define GUARTGETDATABITS
#define GUARTSETDATABITS

#define GUARTGETSTOP
#define GUARTSETSTOP
#define     GUARTSTOPONE
#define     GUARTSTOPTWO    /* see below */

/* With 16550 type UARTs GUARTSTOPTWO actually only gives one and a */
/* half stop bits whenever the number of data bits is set to five.  */

#define GUARTGETPARITY
#define GUARTSETPARITY
#define     GUARTPARITYNONE
#define     GUARTPARITYODD
#define     GUARTPARITEVEN
#define     GUARTPARITYSTICKZERO
#define     GUARTPARITYSTICKONE

#endif
/***************************************************/

#define GUARTRXOVER 0x41
#define GUARTPARITY 0x42
#define GUARTFRAME  0x43
#define GUARTBREAK  0x44
#define GUARTGTIME  0x50
#define GSCIESCERR  0x71

#define GUARTSTAT_ERROR 0x01

