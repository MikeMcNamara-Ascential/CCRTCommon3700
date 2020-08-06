// supplied by Dearborn Group.
// we are using this file

#ifndef gdev_h
#define gdev_h

/* $Id: gdev.h,v 1.68 2002/08/09 21:26:57 stevelim Exp $ */

#include "gen_ioctls.h"

/* Card specific information */

#include "hw/dev_527.h"
#include "hw/dev_scp.h"
#include "hw/dev_dlc.h"
#include "hw/dev_kwp.h"
#include "hw/dev_ubp.h"
#include "hw/dev_hon.h"
#include "hw/dev_uart.h"
#include "hw/dev_sja.h"
#include "hw/dev_lin.h"

/* Gryphon device driver parameters */

#define GMAJOR      61
#define CHANSPACING 8

//#define MAXCHANS  32
// reduced for our configuration.
#define MAXCHANS    8
#define DATAMINOR   1
#define INFOMINOR   2
#define CTLMINOR    4
#define STATMONMINOR    5

#define MAXTXTMSGLEN    10240
#define CRITBUFSIZE 101



/* Hardware / driver TYPE and SUBTYPE definitions */


#define GDUMMY      0x01    /* Dummy device driver TYPE */
#define GDGDMARKONE 0x01        /* Dummy device driver SUBTYPE */

#define GCAN        0x02    /* CAN TYPE */
#define G82527      0x01        /* 82527 SUBTYPE */
#define GSJA1000    0x02        /* SJA1000 SUBTYPE */
#define G82527SW    0x03        /* 82527 single wire subtype */
#define G82527ISO11992  0x04        /* 82527 ISO11992 subtype */

/* urgh: */
#define G82527_SINGLECHAN       0x05    /* 82527 single channel */
#define G82527SW_SINGLECHAN     0x06    /* 82527 single wire single channel */
#define G82527ISO11992_SINGLECHAN   0x07    /* 82527 ISO11992 single channel */
#define GSJA1000FT  0x10        /* SJA1000 Fault Tolerant subtype */
#define GSJA1000C   0x11        /* SJA1000 Compact subtype */
#define GSJA1000FT_FO   0x12            /* SJA1000 single channel Fault Tolerant subtype */
#define GSJA1000SW      0x13            /* SJA1000 single wire CAN subtype  */
#define GSJA1000C_SC  	0x14		/* SJA1000 Compact subtype single chan*/
#define GSJA1000SW_2  	0x15		/* SJA1000 Compact subtype dual chan  */


#define GJ1850      0x03    /* 1850 TYPE */
#define GHBCCPAIR   0x01        /* HBCC SUBTYPE */
#define GDLC        0x02        /* GM DLC SUBTYPE */
#define GDEHC12BDLC 0x03        /* DE HC12 KWP/BDLC card SUBTYPE */
#define GHIP7010    0x04        /* intersil HIP7010 SUBTYPE */
#define GHC12BDLC2	0x05		/* Improved BDLC on keyword card */

#define GKWP2000    0x04    /* Keyword protocol 2000 / ISO 9141 TYPE */
#define GISO9141    GKWP2000
#define GDEHC12KWP  0x01        /* DE HC12 KWP/BDLC card SUBTYPE */

#define GHONDA      0x05    /* Honda UART TYPE */
#define GDGHC08     0x01        /* DG HC08 SUBTYPE */

#define GFORDUBP    0x06    /* FORD UBP TYPE */
#define GDGUBP08    0x01        /* DG HC08 SUBTYPE */

#define GGENERALIO  0x07    /* General Input/Output TYPE */
#define GDEIO167    0x01        /* DE 167 IO board SUBTYPE */
#define GDEIOHC12   0x02        /* DE HC12 IO board SUBTYPE */
#define GDGIOPICD	0x03		/* DG PIC Digital IO board SUBTYPE */

#define GUART       0x08    /* Generic UART TYPE */
#define G16550      0x01        /* basic 16550 SUBTYPE */

#define GSCI        0x09    /* Chrysler SCI TYPE */
#define G16550SCI   0x01        /* 16550 type UART based card SUBTYPE */

#define GCCD        0x0a    /* Chrysler C2D TYPE */
#define G16550CDP68HC68 0x01        /* 16550 / CDP68HC68S1 card SUBTYPE */

#define GLIN        0x0b    /* LIN TYPE */
#define GDGLIN08    0x01        /* DG HC08 SUBTYPE */
#define GDGLIN208	0x02		/* DG HC08 SUBTYPE  ver. 2.0        */
#define GDGLIN2x2	0x03		/* Dual channel SUBTYPE  ver. 2.0   */

#define	GVISTNBUS   	0x0c	/* Visteon NBUS TYPE */
#define GNBUS           0x01            /* DG NBUS subtype */


#ifndef _WINDOWS_

#ifdef __KERNEL__
#include <linux/time.h>
#else
#include <sys/time.h>
#endif

struct statreport
    {
    struct timeval tv;
    unsigned long rxtotal;
    unsigned long rxnetbits;
    };

#endif

#endif  //gdev_h
