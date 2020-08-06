// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_sja.h,v 1.8 2002/05/01 18:49:43 stevelim Exp $*/

/* dev_sja.h - sja1000-based CAN board */

/* EVENT definitions (FT_EVENT frames)                              */
/* The same events as defined in dev_527.h are used by the driver   */
/* with the following additions.  (Some of the events defined in    */
/* dev_527.h are not used.                                          */
#define GSJAOTHER   0x50    /* Other error */
#define GSJABIT     0x51    /* Bit error */
#define GSJATRIGSET     0x52    /* I/O trigger */
#define GSJATRIGRESET   0x53    /* I/O trigger */
#define GSJAFT          0x54    /* Fault tolerant change of state */

/* Device driver queue                                              */
/* The same values as defined in dev_527.h are used by the driver.  */

/* IOCTL definitions - comments indicate data size                  */
/* The same defintions as found in dev_527.h are used by the        */
/* driver in addition to those defined below.                       */

#define GSJAGETLISTEN   0x11250001  /* 1 */
#define GSJASETLISTEN   0x11250002  /* 1 mode set on next init */

#define GSJAGETSELFTEST 0x11250003      /* 1 */
#define GSJASETSELFTEST 0x11250004      /* 1 mode set on next init */

#define GSJAGETXMITONCE 0x11250005      /* 1 */
#define GSJASETXMITONCE 0x11250006      /* 1 immediate */

#define GSJAGETTRIGSTATE 0x11250007     /* 1 */

#define GSJASETTRIGCTRL 0x11250008      /* 1 */
#define GSJAGETTRIGCTRL 0x11250009      /* 1 immediate */

#define GSJAGETOUTSTATE 0x1125000A      /* 1 */
#define GSJASETOUTSTATE 0x1125000B      /* 1 immediate */

#define GSJAGETFILTER   0x1125000C      /* 1, 3 or 5 */
#define GSJASETFILTER   0x1125000D      /* 1, 3 or 5 immediate */

#define GSJAGETMASK     0x1125000E      /* 1, 3 or 5 */
#define GSJASETMASK     0x1125000F      /* 1, 3 or 5 immediate */

#define GSJAGETINTTERM  0x11250010      /* 1 */
#define GSJASETINTTERM  0x11250011      /* 1 */

#define GSJAGETFTTRANS  0x11250012      /* 1 */
#define GSJASETFTTRANS  0x11250013      /* 1 immediate */

#define GSJAGETFTERROR  0x11250014      /* 1 */
