/* Scheduler (SD_SCHED) target commands: */

#ifndef sched_h
#define sched_h

#define CMD_SCHED_TX		0x70	/* schedule transmission list */
#define CMD_SCHED_KILL_TX	0x71	/* stop and destroy job */
#define CMD_SCHED_STOP_TX	0x71	/* deprecated */
#define CMD_SCHED_MSG_REPLACE   0x72    /* replace a scheduled message  */


/* GSCHEDDONE should not be in the generic event range but changing	*/
/* the value would break old software					*/
#define GSCHEDDONE		0x04	/* sched job completed */
#define EVENT_SCHED_DONE	0x04	/* sched job completed */

struct gschedentryhdr {
    unsigned long       sleep;
    unsigned long       count;
    unsigned long       period;
    unsigned short      flags;
    unsigned char       chan;
    unsigned char       unused;
};

struct gschedentry {
    struct gschedentryhdr   header;
    struct gdatahdr         data_hdr;
    unsigned char           data[1];
};
#endif
