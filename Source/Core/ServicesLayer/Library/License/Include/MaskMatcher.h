//******************************************************************************
// Description:
//  The MaskMatcher Class. This can be used for comparing
// strings to a mask string. The mask string can contain normal ascii characters,
// with Question Marks and Asterisk characters being interpreted as wildcard
// characters. A question mark is a single character wildcard, and an asterisk is
// a 0 to multiple character wildcard character.
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
//    $Log: /License/LicenseLibraries/Source/Core/License/Include/MaskMatcher.h $
// 
// 4     1/18/06 3:20p Cward
// 
// 2     1/18/06 2:56p Cward
// Updated for common license code
//
// 1     11/23/05 4:59p Cward
// License Library Update for Five by Five and CMake
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MASKMATCHER_H
#define MASKMATCHER_H

#define MAX_MASK_ITEMS 8
#define MAX_MASK_LEN 32

typedef struct
{
	unsigned int questCnt;
	int			 asterisks;
	char		 needle[MAX_MASK_LEN];

} maskItemTyp;

typedef struct
{
	int			valid;
	int			maskItems;
	maskItemTyp compiledMask[MAX_MASK_ITEMS];
} cMaskTyp;

//static maskItemTyp	compiledMask[MAX_MASK_ITEMS];
//static int cMaskItems = 0;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compile Mask parses a mask string and breaks it into mask item
 * components and populates the cMaskTyp structure supplied to
 * define how to compare search strings in the MaskMatches routine.
 *
 * @param mask	a null terminated character array (c style string)
 * 				containing the mask to be compiled.
 * @param mt	a cMaskTyp structure that will contain the results
 * 				of the parsing and compilation of component items
 * @return 1 on success or 0 on failure.
 */
int CompileMask(const char *mask, cMaskTyp *mt );


/**
 * Matches Mask routine applies a compiled mask contained in a
 * cMaskTyp structure to a C type character string to determine
 * if the string matches the mask.
 *
 * @param search	a null termianted character array (c style string)
 * 					containing the string to compare against the mask.
 * @param mt		a cMaskTyp structure that contains the compiled
 * 					mask to be checked
 * @return 1 on success or 0 on failure. Success in this case means
 * 			that there were no processing errors, and that the search
 * 			string matches the supplied mask.
 */
int MatchesMask(const char *search, cMaskTyp *mt );


#ifdef __cplusplus
}
#endif

#endif
