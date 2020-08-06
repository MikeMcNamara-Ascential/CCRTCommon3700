// supplied by Dearborn Group.
// we are using this file

#ifndef msgresp_h
#define msgresp_h

/* $Id: msgresp.h,v 1.2 2000/05/25 16:07:04 stevelim Exp $ */

/* Message responder target commands,structures */


/* SD_RESP command types: */
#define CMD_MSGRESP_ADD     0xB0
#define CMD_MSGRESP_GET     0xB1
#define CMD_MSGRESP_MODIFY      0xB2
#define CMD_MSGRESP_GET_HANDLES 0xB3

/* Actions available via CMD_MODIFY_RESPONSE */
#define DELETE_RESPONSE         0
#define ACTIVATE_RESPONSE       1
#define DEACTIVATE_RESPONSE     2

#define EVENT_RESPONSE          0x42    /* Message Response events */

/* Flags to modify how FT_CMD (command) messages are handled            */
/* These values are ORed with FT_CMD and stored in the Frame Header's   */
/* Frame Type field for each response.                                  */
#define DONT_WAIT_FOR_RESP      0x80
#define WAIT_FOR_PREV_RESP      0x40
#define RESPONSE_FLAGS      (DONT_WAIT_FOR_RESP | WAIT_FOR_PREV_RESP)

struct gfrmresphdr
    {
    unsigned char   flags;
    unsigned char   num_fields;
    unsigned char   num_responses;
    unsigned char   old_handle;
    unsigned char   action;
    unsigned char   reserved;
    unsigned short  value;
};


struct gresponse {
    struct gfrmresphdr  header;
    struct gfiltbdy     body;
};

#endif  // msgresp_h
