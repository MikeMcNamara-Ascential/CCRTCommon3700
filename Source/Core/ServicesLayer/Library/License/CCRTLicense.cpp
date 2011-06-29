//******************************************************************************
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/License/CCRTLicense.cpp $
// 
// 2     3/02/06 12:41a Cward
// Updated for 5x5 Licensing
//
// 1     2/16/05 9:01p Cward
// Initial Version
//
// 1     1/06/05 3:53p Cward
//
// 4     6/24/04 12:59a Cward
// Removed pound if 0 block. Updated the header block comments to the new
// standard.
//
// 3     5/19/04 9:38p Cward
// Added Read Mac Address from file to support the number reader and
// License Creator tools. Removed the usage section as it should be part
// of the main programs
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "CCRTLicense.h"
#include "License.h"

/**
 * CCRTLicense constructor
 *
 */
CCRTLicense::CCRTLicense()
{
}

/**
 * CCRTLicense destructor
 */
CCRTLicense::~CCRTLicense()
{
}

/**
 * Checks the validity of the license
 *
 * @return true if license valid; false otherwise.
 */
bool CCRTLicense::Valid(void)
{
	int isok = IsLicenseValid("en0", "keyfile.dat", "*.lic", "/config/license", "01" );

	return(isok == 1);
}


