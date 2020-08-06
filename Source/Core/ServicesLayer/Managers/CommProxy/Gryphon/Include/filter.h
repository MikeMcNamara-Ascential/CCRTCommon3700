// supplied by Dearborn Group.
// we are using this file

#ifndef filter_h
#define filter_h

/* $Id: filter.h,v 1.5 2001/09/28 19:11:52 stevelim Exp $ */

/* Filter structures and definitions */


struct gfilthdr
    {
    unsigned char   flags;
    unsigned char   num_fields;
    unsigned short  reserved;
    unsigned long   reserved1;
};
#define FILTER_PASS_FLAG            0x01
#define FILTER_ACTIVE_FLAG          0x02
#define FILTER_OR_BLOCKS_FLAG       0x04
#define FILTER_FLAG_BLOCK           0x00
#define FILTER_FLAG_PASS            FILTER_PASS_FLAG
#define FILTER_FLAG_INACTIVE        0x00
#define FILTER_FLAG_ACTIVE          FILTER_ACTIVE_FLAG
#define FILTER_FLAG_AND_BLOCKS      0x00
#define FILTER_FLAG_OR_BLOCKS       FILTER_OR_BLOCKS_FLAG

struct gfiltbdy {
    unsigned short  offset;
    unsigned short  length;
        unsigned char   data_type;
    unsigned char   op;
        unsigned char   step_size;
    unsigned char   reserved;
    unsigned char   values[30];
};
#define FILTER_DATA_TYPE_HEADER_FRAME   0x00
#define FILTER_DATA_TYPE_HEADER         0x01
#define FILTER_DATA_TYPE_DATA           0x02
#define FILTER_DATA_TYPE_EXTRA_DATA     0x03
#define FILTER_EVENT_TYPE_HEADER    0x04
#define FILTER_EVENT_TYPE_DATA          0x05

struct gfilter {
    struct gfilthdr header;
    struct gfiltbdy body;
};

struct FilterMessage
{
    struct gfilthdr FHead;
    struct gfiltbdy FBlock[8];
};



/* Actions available via CMD_CARD_MODIFY_FILTER */
#define DELETE_FILTER           0
#define ACTIVATE_FILTER         1
#define DEACTIVATE_FILTER       2

/* Modes available via CMD_CARD_SET_FILTERING_MODE */
#define FILTER_OFF_PASS_ALL     3
#define FILTER_OFF_BLOCK_ALL    4
#define FILTER_ON               5

/* Modes available via CMD_CARD_SET_DEFAULT_FILTER */
#define DEFAULT_FILTER_BLOCK    0
#define DEFAULT_FILTER_PASS     1

/* Filter and Frame Responder Condition operators */
#define BIT_FIELD_CHECK         0
#define SVALUE_GT               1
#define SVALUE_GE               2
#define SVALUE_LT               3
#define SVALUE_LE               4
#define VALUE_EQ                5
#define VALUE_NE                6
#define UVALUE_GT               7
#define UVALUE_GE               8
#define UVALUE_LT               9
#define UVALUE_LE               10
#define DIG_LOW_TO_HIGH         11
#define DIG_HIGH_TO_LOW         12
#define DIG_TRANSITION          13

#endif  //filter_h
