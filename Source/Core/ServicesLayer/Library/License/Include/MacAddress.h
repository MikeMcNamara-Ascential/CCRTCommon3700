//******************************************************************************
// Description:
//		Common platform interface for getting Mac Address information.
//
//==============================================================================
// Copyright (c) 2004 - 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /License/LicenseLibraries/Source/Core/License/Include/MacAddress.h $
// 
// 4     1/18/06 3:20p Cward
// 
// 2     1/18/06 2:56p Cward
// Updated for common license code
//
// 1     11/23/05 4:59p Cward
// License Library Update for Five by Five and CMake
//
// 1     1/06/05 3:53p Cward
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MACADDRESS_H
#define MACADDRESS_H
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the MAC Address
 *
 * @param	buf - a pointer to a character array of at least 6 characters
 * 				  that will be populated with the Mac Address.
 *
 * @param	ifc - name of the NIC adapter interface to be used
 * 				  examples are "en0", "en1", "Local Area Connection" etc.
 *
 * @return 1 if successful; 0 otherwise.
 */
int GetMACAddress(char *buf, const char *ifc);

/**
 * Enable MAC Address Debug
 *
 * This routine turns on console print messages
 * to aid in the debug of MAC Address routines.
 */
void EnableMACAddressDebug(void);


#ifdef __cplusplus
}
#endif



#endif // MACADDRESS_H

