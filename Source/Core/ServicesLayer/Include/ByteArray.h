//******************************************************************************
// Description:
//    Object used to manage an array of raw bytes
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Core/ServicesLayer/Include/ByteArray.h $
// 
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
//
// 1     6/11/04 4:15p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:11a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 1     12/23/03 8:15p Bmeinke
// Initial checkin
//
// 1     12/23/03 8:13p Bmeinke
// Initial checkin
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _BYTEARRAY_H_INCLUDED_
#define _BYTEARRAY_H_INCLUDED_

#include <stdint.h>
#include <string>

typedef std::basic_string< uint8_t>		ByteArray_t;
typedef ByteArray_t::iterator			ByteArrayItr_t;
typedef ByteArray_t::const_iterator		ByteArrayCItr_t;


#endif // _BYTEARRAY_H_INCLUDED_

