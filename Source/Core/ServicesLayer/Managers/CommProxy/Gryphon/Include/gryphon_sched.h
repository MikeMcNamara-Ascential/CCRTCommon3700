// not used at this time

/* $Id: sched.h,v 1.1 1999/10/04 21:40:29 jason Exp $ */

/* Scheduler (SD_SCHED) target commands: */

#define CMD_SCHED_TX		0x70	/* schedule transmission list */
#define CMD_SCHED_KILL_TX	0x71	/* stop and destroy job */
#define CMD_SCHED_STOP_TX	0x71	/* deprecated */


/* GSCHEDDONE should not be in the generic event range but changing	*/
/* the value would break old software					*/
#define GSCHEDDONE		0x04	/* sched job completed */
#define EVENT_SCHED_DONE	0x04	/* sched job completed */
