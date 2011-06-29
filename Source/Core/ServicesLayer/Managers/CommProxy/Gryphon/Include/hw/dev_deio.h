// supplied by Dearborn Group.
// we are using this file

/* $Id: dev_deio.h,v 1.2 2001/10/04 22:48:17 stevelim Exp $*/

/* dev_deio.h - Dearborn Electronics designed I/O board */

/* EVENT definitions (FT_EVENT frames)                              */

#define GCIOWATCHDOG    0x41        /* dead */
#define GCIOSEMTIMEOUT  0x42        /* not good at all */
#define GCIOFIFOERR 0x43        /* really not good at all */


/* Device driver queue                                              */

#define PACKSIZE    8
#define MAXPACKPERMSG   128 /* see below if changing */
#define MAXDEIOMSG  1024    /* PACKSIZE * MAXPACKPERMSG */
/* MAXDEIOMSG _must_ be smaller than TX fifo, preferably    */
/* significantly smaller                    */

#define DEIOBUFSIZE 32  /* see below if changing */

/* this lookup table is used to decide whether we should bother taking  */
/* stuff from the dual port recieve fifo and sticking it into a single  */
/* gryphon message or wait till theres more to bother with.  It should  */
/* be indexed by the number of messages in the device driver buffer.    */
/* The contents describe the minimum amount of packets in the dual port */
/* rx fifo we should bother with. i.e:                  */
/* if dpbufferfullness < minmsgtable[ddbufferfullness] then do nothing  */
/* The problem its supposed to be addressing is if 'load' is low,   */
/* minimise latency, otherwise save CPU by waiting (if need be) and */
/* packing the most into gryphon messages.              */
/* Tuning this to get the best performance is obviously going to be a   */
/* black art (sorry).  I'm not saying this is the nicest/best method,   */
/* but hey, its a start.                        */
/* Last member shouldn't be needed, due to the fact theres always one   */
/* space left in the queue.  Each entry _must_ be > 0 and should not    */
/* exceed MAXPACKPERMSG, except the last one _must_ be higher than the  */
/* largest size of dprxfifo (yes, I'm saving CPU cycles here).      */
/* The first few entries are of course the most important.  First one   */
/* should be 1 to minimise latency.  Third one should be higher to give */
/* priority to devices with less buffering.             */


static unsigned short minmsgtable[DEIOBUFSIZE]=
    {
     1,  1,  4,  4,  6,  8, 10, 12, 16, 24,
    32, 36, 40, 44, 48, 52, 56, 60, 64, 68,
    72, 76, 80, 84, 88, 92, 94, 98, 102, 106,
    110, 0xffff
    };


/* IOCTL definitions - comments indicate data size                  */


/* Action codes                                                             */

#define GCIOREAD        1       /* Read a value from the I/O card           */
#if 0   /* The following defines are not yet implemented                    */
#define GCIOREADCL      2       /* Read and clear a count value             */
#define GCIOPERIOD      3       /* Define a period for auto reporting       */
#define GCIOEXCEPT      4       /* Define a delta for report by exception   */
#endif
#define GCIOWRITE       5       /* Write a value to the I/O card            */
#define GCIOSETBITS     6       /* Write the indicated bits as 1            */
#define GCIOCLRBITS     7       /* Write the indicated bits as 0            */
#define GCIOTOGBITS     8       /* Write the indicated bits inverted from current*/


/* ID codes                                                                 */
#define GCIODIN         1       /* Digital inputs (16 at a time)            */
#define GCIODINISO      2       /* Optically isolated digital inputs (16)   */
#define GCIODOUT        3       /* Digital outputs (16 at a time)           */
#define GCIOAIN         4       /* Analog input (10 bit value)              */
#if 0   /* The following define is not supported                            */
#define GCIOAINDIF      5       /* Differential analog inputs (12 bit value)*/
#endif
#define GCIOFREQIN      6       /* Frequency input (in 0.1 cps or Hz)       */
#define GCIODCIN        7       /* Duty cycle input (0.00 to 100.00 percent)*/
#if 0   /* The following define is not supported                            */
#define GCIOAIGO        8       /* Differnetial analog gain & offset        */
#endif
#if 0   /* The following define is not yet implemented                      */
#define ASSEMBLAGE      9
#endif
