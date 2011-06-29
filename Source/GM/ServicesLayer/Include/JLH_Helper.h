//*************************************************************************
// FILE:
//  
//
// FILE DESCRIPTION:
//   Base server for all iterative servers.
//
//===========================================================================
// Copyright (c) 2001-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
//*************************************************************************
#ifndef JLH_Helper_h
#define JLH_Helper_h

#ifndef FALSE
    #define FALSE                       0                                                
    #define TRUE                        (!FALSE)
#endif

#ifndef NULL
    #define NULL                        0
#endif

#define UINT8_SHIFT_L(X,Y)              ((X) << (sizeof(UINT8) * 8 * (Y))) 
#define UINT8_SHIFT_R(X,Y)              ((X) >> (sizeof(UINT8) * 8 * (Y)))


#define UINT8_FROM_STRING(X,Y)          ( X = (UINT8)atoi(Y.c_str()) )
#define UINT16_FROM_STRING(X,Y)         ( X = (UINT16)atoi(Y.c_str()) )

#define ABS(X)                          (((X) < 0) ? -(X) : (X))
#define CLAMP(X,L,H)                    (((X) > (H)) ? (H) : (((X) < (L)) ? (L) : (X)))
#define MIN(X,Y)                        (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y)                        (((X) > (Y)) ? (X) : (Y))


inline void SitAndSpin()
{
    bool run = true;
    while(run)
    {
        BposSleep(1000);
    }
}

#endif

