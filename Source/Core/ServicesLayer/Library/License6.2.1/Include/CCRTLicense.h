//******************************************************************************
//
// Description:
//    Utility routines used to read, encrypt and validate a machine's MAC address
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/License/Include/CCRTLicense.h $
// 
// 2     3/02/06 12:41a Cward
// Updated for 5x5 Licensing
//
// 1     2/16/05 9:01p Cward
//
// 1     1/06/05 3:53p Cward
//
// 2     5/19/04 9:39p Cward
// Added Definition of the Read Mac Address From File Method.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/10/03 9:20a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
//
//******************************************************************************

#ifndef CCRTLICENSE_H
#define CCRTLICENSE_H

class CCRTLicense
{
public:

	/**
	 * CCRTLicense constructor
	 *
	 */
	CCRTLicense();

	/**
	 * CCRTLicense destructor
	 */
	virtual ~CCRTLicense();

	/**
	 * Checks the validity of the license
	 *
	 * @return true if license valid; false otherwise.
	 */
	bool Valid(void);

private:


};


#endif // CCRTLICENSE_H

