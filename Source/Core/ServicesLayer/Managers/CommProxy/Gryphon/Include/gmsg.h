// supplied by Dearborn Group.
// we are using this file
#ifndef gmsg_h
#define gmsg_h

/* $Id: gmsg.h,v 1.81 2001/05/07 12:40:15 jason Exp $ */

/*                                  */
/* gmsg.h - generic parts of Gryphon Protocol               */
/*                                  */
/*                                  */
/*                                  */
/* consistancy makes it easier to remember names etc:           */
/*                                  */
/* C variables, function names, types, members of structs - all lower   */
/* case, no underscores.                        */
/*                                  */
/* #defines - all uppercase, underscores permitted.         */
/*                                  */
/* familiar abbreviations such as SRC, DST, MSG, CMD, ERR etc should    */
/* always be used.                          */
/*                                  */
/* ansi C is prefered.                          */
/*                                  */
/* variable length data is always padded to multiples of 4 bytes.   */
/*                                  */

#define PADDING(x)      (3-(((x)+3)%4))


#include "frame.h"
#include "info.h"
#include "filter.h"
#include "g_sched.h"
#include "blm.h"
#include "flight.h"
#include "msgresp.h"
#include "pgmloader.h"
#include "usdt.h"
#include "iopwr.h"
#include "lin.h"
#include "cnvt.h"


#define MAXPAYLOAD      7168    /* max number of bytes of data  */
                    /* or command/response/event    */
                    /* parameters (not inc hdrs)    */

#define MINMSGLEN       0
#define MAXMSGLEN       (((int)sizeof(struct gdatahdr))+MAXPAYLOAD)
#define MINGFRAME       (((int)sizeof(struct gframehdr))+MINMSGLEN)
#define MAXGFRAME       (((int)sizeof(struct gframehdr))+MAXMSGLEN)


/* source/destinations: */

#define SD_CARD         0x01    /* (vehicle) network interface */
#define SD_SERVER       0x02
#define SD_CLIENT       0x03
#define SD_KNOWN        0x10    /* Client ID >= are well known */
#define SD_SCHED        0x10    /* scheduler */
#define SD_SCRIPT       0x20    /* script processor */
#define SD_PGM          0x21    /* Program loader */
#define SD_USDT         0x22    /* USDT (Unacknowledged Segmented Data Transfer) */
#define SD_BLM          0x23    /* Bus Load Monitoring */
#define SD_LIN                  0x24    /* LIN extensions */
#define SD_FLIGHT       0x25    /* Flight Recorder */
#define SD_LOGGER               0x25    /* Data logger */
#define SD_RESP         0x26    /* Message Response */
#define SD_IOPWR		0x27	/* VNG / Compact Gryphon I/O & power */
#define SD_UTIL			0x28	/* Miscellaneous utility commands   */
#define SD_CNVT                 0x29    /* Signal conversion commands       */
#define SD_J1939TP              0x30    /* J1939 Transport Protocol */

#define TP_J1939        SD_J1939TP

/* frame types: */

#define FT_CMD          0x01    /* command to initiate some action */
#define FT_RESP         0x02    /* response to a command */
#define FT_DATA         0x03    /* (vehicle) network data */
#define FT_EVENT        0x04    /* notification of an event */
#define FT_MISC         0x05    /* misc data */
#define FT_TEXT         0x06    /* null-terminated ASCII strings */
#define FT_SIG                  0x07    /* (vehicle) network signals */

#define MAX_TEXT        0xff    /* Maximum FT_TEXT string length */


/* definitions of mode field in FT_DATA frames */

#define MODE_REMOTE     0x10    /* remote message */
#define MODE_LOCAL      0x20    /* local message - obsolete/redundant */
#define MODE_RX         0x40    /* received message */
#define MODE_TX         0x80    /* transmitted message */
#define MODE_INTERNAL   0x01    /* for internal use within gryphon */
#define MODE_NOMUX		0x02	/* message is not to be multiplexed */
#define MODE_COMBINED           0x04    /* channel number is in context */


/* commands:  (see also files included by gdev.h)*/

/* generic (all SD type) commands: values 0x00 to 0x3f */

#define CMD_INIT        	0x01    /* initialize target */
#define CMD_GET_STAT        0x02    /* request status */
#define CMD_GET_CONFIG      0x03    /* request configuration info */
#define CMD_EVENT_ENABLE    0x04    /* Enable event type */
#define CMD_EVENT_DISABLE   0x05    /* Disable event type */
#define CMD_GET_TIME        0x06    /* Get current value of timestamp */
#define CMD_GET_RXDROP      0x07    /* Get count of Rx msgs dropped */
#define CMD_RESET_RXDROP    0x08    /* Set count of Rx msgs dropped to zero */
#define CMD_BCAST_ON        0x09    /* broadcasts on */
#define CMD_BCAST_OFF       0x0a    /* broadcasts off */
#define CMD_SET_TIME        0x0b    /* set time */

/* SD-type specific commands: should start at 0x40, global uniqueness   */
/* is prefered, but not mandatory.                  */

/* SD_CARD command types: */

#define CMD_CARD_SET_SPEED  		0x40    /* set peripheral speed */
#define CMD_CARD_GET_SPEED  		0x41    /* get peripheral speed */
#define CMD_CARD_SET_FILTER 		0x42    /* set filter to pass or block all */
#define CMD_CARD_GET_FILTER 		0x43    /* get a pass/block filter */
#define CMD_CARD_TX     			0x44    /* transmit message */
#define CMD_CARD_TX_LOOP_ON 		0x45    /* set transmit loopback on */
#define CMD_CARD_TX_LOOP_OFF    	0x46    /* set transmit loopback off */
#define CMD_CARD_IOCTL      		0x47    /* device driver ioctl pass-through */
#define CMD_CARD_ADD_FILTER 		0x48    /* add a pass/block filter */
#define CMD_CARD_MODIFY_FILTER  	0x49    /* modify a pass/block filter */
#define CMD_CARD_GET_FILTER_HANDLES 0x4A/* get a list of filters */
#define CMD_CARD_SET_DEFAULT_FILTER 0x4B/* set the default action */
#define CMD_CARD_GET_DEFAULT_FILTER 0x4C/* get the defautl action */
#define CMD_CARD_SET_FILTER_MODE 	0x4D   /* set the client data mode */
#define CMD_CARD_GET_FILTER_MODE 	0x4E   /* get the client data mode */
#define CMD_CARD_GET_EVNAMES     	0x4f   /* get event names */
#define CMD_CARD_GET_SPEEDS  		0x50   /* get speed definitions */

/* SD_SERVER command types: */

#define CMD_SERVER_REG      	0x50    /* register connection */
#define CMD_SERVER_SET_SORT     0x51    /* set sorting behavior */
#define CMD_SERVER_SET_OPT      0x52    /* set type of optimization */
#define CMD_SERVER_SET_TIMED_XMIT   0x53	/* set to time xmit data frame msgs */
#define CMD_SERVER_SET_SERVICE  0x54    /* set the higher-layer protocol service */
#define CMD_J1939_ADDR_CLAIM    0x55    /* claim J1939 address */

/* SD_CLIENT command types: */

#define CMD_CLIENT_GET_ID   0x60    /* get the ID (channel field) of this client? */
#define CMD_CLIENT_SET_ID   0x61    /* set the ID (channel field) of this client? */
#define CMD_CLIENT_SHUTDOWN 0x62    /* tell client to die ? */


/* SD_SCRIPT command types: */

#define CMD_SCRIPT_ADD      0x80    /* Add a script. */
#define CMD_SCRIPT_DEL      0x81    /* Delete a script */
#define CMD_SCRIPT_START    0x82    /* Start script execution */
#define CMD_SCRIPT_STOP     0x83    /* Stop script execution */

/* SD_UTIL command types:   */
#define CMD_UTIL_SET_INIT_STRATEGY 0x90 /* set the initialization strategy  */
#define CMD_UTIL_GET_INIT_STRATEGY 0x91 /* get the initialization strategy  */


/* response frame (FT_RESP) response field definitions: */

#define RESP_OK         	0x00    /* no error */
#define RESP_UNKNOWN_ERR    0x01    /* unknown error */
#define RESP_UNKNOWN_CMD    0x02    /* unrecognised command */
#define RESP_UNSUPPORTED    0x03    /* unsupported command */
#define RESP_INVAL_CHAN     0x04    /* invalid channel specified */
#define RESP_INVAL_DST      0x05    /* invalid destination */
#define RESP_INVAL_PARAM    0x06    /* invalid parameters */
#define RESP_INVAL_MSG      0x07    /* invalid message */
#define RESP_INVAL_LEN      0x08    /* invalid length field */
#define RESP_TX_FAIL        0x09    /* transmit failed */
#define RESP_RX_FAIL        0x0a    /* receive failed */
#define RESP_AUTH_FAIL      0x0b
#define RESP_MEM_ALLOC_ERR  0x0c    /* memory allocation error */
#define RESP_TIMEOUT        0x0d    /* command timed out */
#define RESP_UNAVAILABLE    0x0e
#define RESP_BUF_FULL       0x0f    /* buffer full */
#define RESP_NO_SUCH_JOB    0x10
#define RESP_NO_ROOM        0x11    /* not enough room on the disk */
#define RESP_BUSY               0x12    /* device or object is busy */


/* event types: */

/* values 0x00 to 0x3f are reserved for generic events  */

#define EVENT_INIT  0x01    /* channel inited */
#define EVENT_SPD   0x02    /* channel speed change */
#define EVENT_CLIENT_GONE       0x03    /* a non well known client has closed its
                                           connection to the server             */
#define EVENT_MSG_SENT      0x04	/* a marked message has been sent */
#define EVENT_ADDR_LOST         0X05    /* The claimed J1939 address has been lost*/

/* See gdev.h included files for card-specific events */


#define OPT_THRUPUT     0
#define OPT_LATENCY     1

/* Special channel ID for broadcast messages */

#define CH_BROADCAST    255


/* This is a nicer way of defining the message than the struct Msg below.    */
/* Most of the code still uses struct Msg and creates pointers to different  */
/* types of subheaders.  New code will probably use this struct Msg instead. */

struct Msg {
    struct gframehdr fh;    
    union {
        struct {
            struct gcmdhdr header;
            unsigned char  data[MAXPAYLOAD];
        } command;
        struct {
            struct gdatahdr header;
            unsigned char data[MAXPAYLOAD];
        } data;
        struct {
            struct geventhdr header;
            unsigned char data[MAXPAYLOAD];
        } event;
        struct {
            struct gresphdr header;
            unsigned char data[MAXPAYLOAD];
        } response;
        struct {
            unsigned char data[MAXPAYLOAD];
        } misc;
        struct {
            unsigned char data[MAXPAYLOAD];
        } text;
    } subhdr;
};

#define FH fh

#define DH subhdr.data.header
#define EH subhdr.event.header
#define RH subhdr.response.header
#define CH subhdr.command.header

#define DD subhdr.data.data
#define ED subhdr.event.data
#define RD subhdr.response.data
#define CD subhdr.command.data
#define TD subhdr.text.data

#endif  //gmsg_h
