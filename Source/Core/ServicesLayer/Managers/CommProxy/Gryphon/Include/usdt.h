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
#define CMD_USDT_SET_STMIN_FC 0xB3
#define CMD_USDT_GET_STMIN_FC 0xB4
#define CMD_USDT_SET_BSMAX_FC 0xB5
#define CMD_USDT_GET_BSMAX_FC 0xB6
#define CMD_USDT_REGISTER_NON_LEGACY 0xB7
#define CMD_USDT_SET_STMIN_OVERRIDE 0xB8
#define CMD_USDT_GET_STMIN_OVERRIDE 0xB9
#define CMD_USDT_ACTIVATE_STMIN_OVERRIDE 0xBA

/* bit definitions for register actions */
#define USDT_ACTION_11_BIT_ONLY 0x00 // bit 1&2
#define USDT_ACTION_29_BIT_ONLY 0x02 // bit 1&2
#define USDT_ACTION_11_BIT_AND_29_BIT 0x04 // bit 1&2

/* bit definitions for USDT register Tx options */
#define USDT_TX_ECHO_LONG_MSGS 0x01 // bit 0
#define USDT_TX_PAD_MSGS_00 0x00    // bit 1&2
#define USDT_TX_PAD_MSGS_FF 0x02    //  bit 1&2
#define USDT_TX_PAD_MSGS_NONE 0x04    // bit 1&2
#define USDT_TX_SEND_EVENT_TX_DONE 0x08 // bit 3
#define USDT_TX_ECHO_SHORT_MSGS 0x10 // bit 4
#define USDT_TX_SEND_RX_NTH_FLOWCONTROL_EVENT 0x20 // bit 5 Send a USDT_RX_NTH_FLOWCONTROL event when the 2nd, 3rd, etc. flow control message is received when transmitting a long message.
/* bit definitions for USDT register Rx options */
#define USDT_RX_SEND_NO_MSGS_OR_EVENTS 0x00 // bit 0&1
#define USDT_RX_SEND_MSGS_AND_EVENTS 0x01 // bit 0&1
#define USDT_RX_SEND_EVENTS_ONLY 0x02 // bit 0&1
#define USDT_RX_SEND_FIRSTFRAME_EVENT 0x04 // bit 2
#define USDT_RX_SEND_LASTFRAME_EVENT 0x08 // bit 3
#define USDT_RX_SEND_TX_NTH_FLOWCONTROL_EVENT 0x20 // bit 5 Send a USDT_TX_NTH_FLOWCONTROL event when the 2nd, 3rd, etc. flow control message is sent when receiving a long message.

/* USDT event, EVENT_USDT */
#define USDT_DONE   1
#define USDT_SEQ_ERR    2
#define USDT_TIMEOUT    3
#define USDT_FIRSTFRAME 4
#define USDT_LASTFRAME  5
#define USDT_TX_NTH_FLOWCONTROL  6  // event for transmit of the nth flow control
#define USDT_RX_NTH_FLOWCONTROL  7  // event for receipt of the nth flow control
/*
 * The following constant name and value are reserved for use by the
 * Gryphon dll function dgUSDTNode::ReportEvent () to turn error
 * event reporting on and off.  (It is not currently possible to
 * independently turn sequence and timeout events on or off.)

#define USDT_ERROR     255

 */


#define EVENT_USDT              0x40    /* USDT events */
#define MAX_USDT_CLIENTS        32      /* Maximum number of simultaneous
                                           USDT clients                     */
#define MAXCLIENTS              32      /* Maximum number of Gryphon clients*/

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
