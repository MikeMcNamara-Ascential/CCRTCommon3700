//*************************************************************************
// FILE:
//    $Header: /CMake Core/Source/Ford/ServicesLayer/Include/DvtProcess.h 2     
//
// FILE DESCRIPTION:
//   message structure descriptions for Ford Dvt messages
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef DVTProcess_h
#define DVTProcess_h

#include "JLH_Helper.h"
                      
const string valueTrue("1");
const string valueFalse("0");

const string defaultPhase("None");

const UINT8 leftFront  = 0;
const UINT8 rightFront = 1;
const UINT8 leftRear   = 2;
const UINT8 rightRear  = 3;
const UINT8 leftTandem = 4;
const UINT8 rightTandem= 5;

// Pulse definitions used to signal that we received a bad message from DVT
#define DVT_PULSE_CODE	            SYSTEM_PULSE+1

#define DVT_MAX_LENGTH              0xFF
#define DVT_LENGTH_OFFSET           0x55
#define DVT_LENGTH_HEADER_FOOTER    0x03
#define DVT_LENGTH                  (DVT_LENGTH_OFFSET - DVT_LENGTH_HEADER_FOOTER)  

//DVT pulse tags (INT32)
//masks if you need them
#define DVT_PULSE_DECODE_VALUE      0x00000000
#define DVT_PULSE_DECODE_MODE       0x000000FF 
#define DVT_PULSE_DECODE_CODE       0x0000FF00 
#define DVT_PULSE_DECODE_DATA       0x00FF0000 
#define DVT_PULSE_DECODE_FLAG       0xFF000000
//flags 
#define DVT_PULSE_DECODE_ACK        0x01  
#define DVT_PULSE_DECODE_NAK        0x02
#define DVT_PULSE_DECODE_START      0x03
#define DVT_PULSE_DECODE_STOP       0x04

struct DVTCommand
{

    UINT8 mode;
    UINT8 code;
    UINT8 data;
    UINT8 flag;
};

//make config?
#define DVT_IGNORE                  0xFF

struct faultMsg{

};

// incoming message from DVT
struct DVTInMsg
{

    UINT8 id;
    UINT8 length;
    UINT8 mode;
    UINT8 command;
    //...
    UINT8 buffer[DVT_MAX_LENGTH-4];   //max length of rest of message
};

//out going data
struct DVTOutData
{
    //xml based
    UINT8   id;
    //named data broker data
    UINT16  PLCin[22];
    UINT16  PLCout[22];

    UINT8   promptCurrent;
    UINT16  icmIndex;

    UINT8   testComplete[2];
    UINT8   testState[6];

    //read dev file data
    float   rollSpeed[6];
    float   rollForce[6];
    float   rollAccel[6];    
};


#endif // include wrapper
