//******************************************************************************
// Description:
//		C Callable interface to the License Class to perform licensing operations.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /License/LicenseLibraries/Source/Core/License/Include/License.h $
// 
// 4     1/18/06 3:20p Cward
// 
// 2     1/18/06 2:56p Cward
// Updated for common license code
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef LICENSE_H
#define LICENSE_H
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks the validity of the license
 *
 * @param	ifc 		name of the NIC adapter interface to be used examples are "en0",
 * 						"en1", "Local Area Connection". if NULL then "en0" will be the
 * 						default on QNX platforms and "Local Area Connection" will
 * 						be the default on Windows Platforms.
 *
 * @param	kynm		filename for the keyfile to be created if the license
 * 						is not valid. if NULL, the default is "keyfile.dat"
 *
 * @param	mskval		file mask to filter files in 5x5 licensing. If NULL the default
 * 						is "*.lic".
 *
 * @param	dirpth		path to the directory where the license files are located. If NULL
 * 						the default is "/config/license" on QNX and "C:\config\license"
 * 						on Windows platforms. If the path does not exist, the routine will
 * 						attempt to create the path. If the directory (path) is created, the
 * 						routine will search all known locations for exiting licenses, and
 * 						copy them to the new directory.
 *
 * @param	allwdval	A string indicating which generations of the license files will
 * 						be allowed. If NULL Defaults to "1". An empty string will allow any
 * 						valid version of a license to be used. A String containing the
 * 						Gen numbers allowed, for instance "12" or "1,2" etc can be used to
 * 						indicate that gens 1 and 2 are allowed.
 *
 * @return 1 if license valid; 0 otherwise.
 */
int IsLicenseValid(const char *ifc,
				   const char *kynm,
				   const char *mskval,
				   const char *dirpth,
				   const char *allwdval );


#ifdef __cplusplus
}
#endif



#endif
