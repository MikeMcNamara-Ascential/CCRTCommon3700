// supplied by Dearborn Group.
// we are using this file

#ifndef info_h
#define info_h

/* $Id: info.h,v 1.1 1999/10/04 21:40:29 jason Exp $ */

/* CMD_GET_INFO structure definitions */

struct serverinfo 
    {
    char name[20];
    char version[8];
    char serial[20];
    unsigned char numdevs;
    unsigned char pada;
    unsigned char padb;
    unsigned char padc;
    char padding[12];
    };

/* Notes on deviceinfo struct:
   vheader contains 32 bits; each bit set represents a valid header
   length (for 0 to 31 byte headers). 
   Example: vheader=0x0013: 4, 1, and 0 byte headers valid
   vdatamax/vdatamin indicate max/min lengths for valid data fields
   vextramax/vextramin indicate max/min lengths for valid extra fields
*/

// also note that fields of non-character type MIGHT be byte-swapped.
// when received over the ethernet, depening on processor.

struct deviceinfo
{
    char name[20];
    char version[8];
    char security[16];
    unsigned long vheader;
    unsigned short vdatamax;
    unsigned short vdatamin;
    char serial[20];
    unsigned char type;
    unsigned char subtype;
    unsigned char channel;
    unsigned char slot;
    unsigned short vextramax;
    unsigned short vextramin;
};

#endif  //info_h
