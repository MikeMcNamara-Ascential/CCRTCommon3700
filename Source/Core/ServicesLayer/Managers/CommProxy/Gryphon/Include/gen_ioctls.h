// supplied by Dearborn Group.
// we are using this file

/* $Id: gen_ioctls.h,v 1.2 1999/10/15 05:23:21 cvs Exp $ */

/* gen_ioctls.h - Generic IOCTL definitions */


#define GSINGLEMSGRDON  0x2babc001  /* 0 */
#define GSINGLEMSGRDOFF 0x2babc002  /* 0 */
#define GSETPERIOD  0x2cabc002  /* 8 */
#define     GPERIODOFF  0
#define     GPERIODMSG  1
#define     GPERIODTIME 2

#define GINIT       0x11100001  /* 0 */
#define GLOOPON     0x11100002  /* 0 */
#define GLOOPOFF    0x11100003  /* 0 */
#define GGETHWTYPE  0x11100004  /* 2 */
#define GGETREG     0x11100005  /* 2 */
#define GSETREG     0x11100006  /* 2 */
#define GGETRXCOUNT 0x11100007  /* 4 */
#define GSETRXCOUNT 0x11100008  /* 4 */
#define GGETTXCOUNT 0x11100009  /* 4 */
#define GSETTXCOUNT 0x1110000a  /* 4 */
#define GGETRXDROP  0x1110000b  /* 4 */ 
#define GSETRXDROP  0x1110000c  /* 4 */
#define GGETTXDROP  0x1110000d  /* 4 */
#define GSETTXDROP  0x1110000e  /* 4 */
#define GGETRXBAD   0x1110000f  /* 4 */ 
#define GGETTXBAD   0x11100011  /* 4 */
/* 
#define GSETTXBAD   0x11100012  not provided 
#define GSETRXBAD   0x11100010  not provided */

#define GGETCOUNTS  0x11100013  /* 48 */
#define GGETBLMON   0x11100014  /* 1 */
#define GSETBLMON   0x11100015  /* 1 */
#define GGETERRLEV  0x11100016  /* 1    [01] */
#define GSETERRLEV  0x11100017  /* 1    [01] vsoni */
#define GGETBITRATE 0x11100018  /* 4 */
#define GGETRAM     0x11100019  /* 3 */
#define GSETRAM     0x1110001a  /* 3 */
