// provided by Dearborn Group

#ifndef usdt_h
#define usdt_h

/* $Id: usdt.h,v 1.5 2002/04/18 20:51:22 stevelim Exp $ */

/* USDT (SD_USDT) target definitions */


/* SD_USDT command types: */
#define CMD_USDT_REGISTER       0xB0
#define CMD_USDT_SET_FUNCTIONAL 0xB1
#define CMD_USDT_SET_EXTENDED   0xB1
#define CMD_USDT_SET_STMIN_MULT 0xB2


/* USDT event, EVENT_USDT */
#define USDT_DONE   1
#define USDT_SEQ_ERR    2
#define USDT_TIMEOUT    3
#define USDT_FIRSTFRAME 4
#define USDT_LASTFRAME  5


#define EVENT_USDT              0x40    /* USDT events */
#define MAX_USDT_CLIENTS        32      /* Maximum number of simultaneous USDT clients */

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// extensions by gswope  - 3/20/3

typedef enum
{
    /**
     * This receive mode will ignore all incoming long messages.  It does not 
     * validata messages, relay the content, nor report any receive events.
     */
    RECV_IGNORE  = 0x00,
    /**
     * This provides normal operation.  Received long messages are evaluated for
     * proper construction, Events are reported, and valid messages are relayed
     * to the client.
     */
    RECV_NORMAL  = 0x01,
    /**
     * Incoming long messages are evaluated and events are passed to the clinet.
     * However, the data purged and not relayed to the client.
     */
    RECV_SILENT  = 0x02
}  ReceiveDataOption_t ;

#endif  //usdt_h
