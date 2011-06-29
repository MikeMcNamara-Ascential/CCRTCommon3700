// written at bepco
// we are using this file

#include "gryphon2.h"
#include "gmsg.h"

//---------------------------------------------------------------------------
// cool stuff to make things easier

// for the most part, these are kind of "reverse" Macros, originally used for
// the windows (builder) implementation, to correlate menu index to selection

char FrameType[][10] = {
    "Unknown",          // 0
    "Command",          // 1
    "Response",         // 2
    "Data",             // 3
    "Event",            // 4
    "Misc",             // 5
    "Text"              // 6
};

char ReturnType[][16] = {
//  "1234567890123456"
    "OK",               // 00
    "KNOWN_ERR",        // 0x01 /* unknown error */
    "UNKNOWN_CMD",      // 0x02 /* unrecognised command */
    "UNSUPPORTED",      // 0x03 /* unsupported command */
    "INVAL_CHAN",       // 0x04 /* invalid channel specified */
    "INVAL_DST",        // 0x05 /* invalid destination */
    "INVAL_PARAM",      // 0x06 /* invalid parameters */
    "INVAL_MSG",        // 0x07 /* invalid message */
    "INVAL_LEN",        // 0x08 /* invalid length field */
    "TX_FAIL",          // 0x09 /* transmit failed */
    "RX_FAIL",          // 0x0a /* receive failed */
    "AUTH_FAIL",        // 0x0b
    "MEM_ALLOC_ERR",    // 0x0c /* memory allocation error */
    "TIMEOUT",          // 0x0d /* command timed out */
    "UNAVAILABLE",      // 0x0e
    "BUF_FULL",         // 0x0f /* buffer full */
    "NO_SUCH_JOB",      // 0x10
    "NO_ROOM"           // 0x11 /* not enough room on the disk */
};

// in the order presented in the list
unsigned char srcTbl[] = {
    SD_SERVER,          // 2
    SD_CARD,            // 1
    SD_CLIENT,          // 3
    SD_SCHED,           // 0x10
    SD_SCRIPT,          // 0x20
    SD_PGM,             // 0x21
    SD_BLM,             // 0x23
    SD_USDT,            // 0x22
    SD_FLIGHT,          // 0x25
    SD_RESP,            // 0x26
    SD_KNOWN,           // 0x10
    SD_KNOWN            // 0x10
};

// in the order presented in the list
unsigned char modeTbl[] = {
    MODE_REMOTE,    // 0x10  remote message
    MODE_LOCAL,     // 0x20  local message - obsolete/redundant
    MODE_RX,        // 0x40  received message
    MODE_TX,        // 0x80  transmitted message
    MODE_INTERNAL   // 0x01  for internal use within gryphon

};

// in the order presented in the list
unsigned char cmdTbl[] ={

// generic commands
    CMD_INIT,               // 0x01 initialize target
    CMD_GET_STAT,           // 0x02 request status
    CMD_GET_CONFIG,         // 0x03 request configuration info
    CMD_EVENT_ENABLE,       // 0x04 Enable event type
    CMD_EVENT_DISABLE,      // 0x05 Disable event type
    CMD_GET_TIME,           // 0x06 Get current value of timestamp
    CMD_GET_RXDROP,         // 0x07 Get count of Rx msgs dropped
    CMD_RESET_RXDROP,       // 0x08 Set count of Rx msgs dropped to zero
    CMD_BCAST_ON,           // 0x09 broadcasts on
    CMD_BCAST_OFF,          // 0x0a broadcasts off
    CMD_SET_TIME,           // 0x0b set time

// card commands
    CMD_CARD_SET_SPEED,            // 0x40 set peripheral speed
    CMD_CARD_GET_SPEED,            // 0x41 get peripheral speed
    CMD_CARD_SET_FILTER,           // 0x42 (depricated) set filter to pass or block all
    CMD_CARD_GET_FILTER,           // 0x43 get a pass/block filter
    CMD_CARD_TX,                   // 0x44 transmit message
    CMD_CARD_TX_LOOP_ON,           // 0x45 set transmit loopback on
    CMD_CARD_TX_LOOP_OFF,          // 0x46 set transmit loopback off
    CMD_CARD_IOCTL,                // 0x47 device driver ioctl pass-through
    CMD_CARD_ADD_FILTER,           // 0x48 add a pass/block filter
    CMD_CARD_MODIFY_FILTER,        // 0x49 modify a pass/block filter
    CMD_CARD_GET_FILTER_HANDLES,   // 0x4A get a list of filters
    CMD_CARD_SET_DEFAULT_FILTER,   // 0x4B set the default action
    CMD_CARD_GET_DEFAULT_FILTER,   // 0x4C get the defautl action
    CMD_CARD_SET_FILTER_MODE,      // 0x4D set the client data mode
    CMD_CARD_GET_FILTER_MODE,      // 0x4E get the client data mode
    CMD_CARD_GET_EVNAMES,          // 0x4f get event names
    CMD_CARD_GET_SPEEDS,           // 0x50 get speed definitions

// server commands
    CMD_SERVER_REG,         // 0x50 register connection
    CMD_SERVER_SET_SORT,    // 0x51 set sorting behavior
    CMD_SERVER_SET_OPT,     // 0x52 set type of optimization

// client commands
    CMD_CLIENT_GET_ID,      // 0x60 get the ID (channel field) of this client?
    CMD_CLIENT_SET_ID,      // 0x61 set the ID (channel field) of this client?
    CMD_CLIENT_SHUTDOWN,    // 0x62 tell client to die ?

// script commands
    CMD_SCRIPT_ADD,         // 0x80 Add a script.
    CMD_SCRIPT_DEL,         // 0x81 Delete a script
    CMD_SCRIPT_START,       // 0x82 Start script execution
    CMD_SCRIPT_STOP         // 0x83 Stop script execution
};

unsigned char FilterModeVals[] ={
    FILTER_OFF_BLOCK_ALL,  // 4 - silent
    FILTER_ON,             // 5 - normal operation
    FILTER_OFF_PASS_ALL,   // 3 - promiscuous mode
};

