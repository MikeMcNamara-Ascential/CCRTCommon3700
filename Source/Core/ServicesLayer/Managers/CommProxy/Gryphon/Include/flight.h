// not used at this time.

/* $Id: flight.h,v 1.1 1999/10/04 21:40:29 jason Exp $ */

/* Flight recorder (SD_FLIGHT) target definitions */

#define FR_RESP_AFTER_EVENT         0
#define FR_RESP_AFTER_PERIOD        1
#define FR_IGNORE_DURING_PER        2
#define FR_DEACT_AFTER_PER          128
#define FR_DEACT_ON_EVENT           64
#define FR_DELETE                   32
#define FR_PERIOD_MSGS              16


#define CMD_FLIGHT_GET_CONFIG   0x50    /* get flight recorder channel info */
#define CMD_FLIGHT_START_MON    0x51    /* start flight recorder monitoring */
#define CMD_FLIGHT_STOP_MON     0x52    /* stop flight recorder monitoring */


#define EVENT_FLIGHT            0x41    /* Flight Recorder events */
