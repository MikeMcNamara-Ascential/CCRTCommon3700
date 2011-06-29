//******************************************************************************
//
// FILE:    XmlDefs.h
//     $Archive:: /Core/ServicesLayer/Include/XmlDefs.h                       $
//     $Author: Cward $
//     $Date: 8/18/04 11:19p $
//
// Description:
//  Defines and type definitions for the BEP XML Parser/Processor
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Core/ServicesLayer/Include/XmlDefs.h $
// 
// 2     8/18/04 11:19p Cward
// changed angle brackets to quotes for included non system files
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:27p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:49  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:11a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 4     6/19/02 5:04p Bmeinke
// Added std namespace qualifier to XmlNodeMapType typedef (std::map)
//
// 3     8/20/01 12:07p Brian.meinke
// Changed include from "xmlparse.h" to <xmlparse.h> to use OS XML libs
//
// 2     8/03/01 10:59p Brian.meinke
// Added #include <sys/neutrino.h> to get OS version define
// Added conditional include of <ostream> based on OS version
//
// 1     4/10/01 10:46a Admin
//
// 6     3/19/01 9:03a Brian.meinke
// Changes to try to use the gnu GCC compiler
//
// 5     3/13/01 1:51p Brian.meinke
// Added include <tchar.h> for generic character manipulation routines
// (tcs* functions)
//
// 4     3/09/01 4:39p Kevin.hykin
//
// 3     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 5     12/18/00 10:48a Brian.meinke
// Changed '#ifdef WATCOM' to  '#ifdef __WATCOMC__'
//
// 4     12/18/00 10:23a Brian.meinke
// Fix for Watcom not liking the 'using namespace std' line
//
// 3     12/12/00 3:13p Brian.meinke
// Updates for QNX/Watcom 10.6:
// Watcom doesn't like exceptions
// Revised declarations for XmlString and XmlOStream
// Overloaded issapce function inside XmlParser class for unicode.ascii
// Miscellaneous other Watcom quirks
//
// 2     12/11/00 10:32a Brian.meinke
// Added additional parameters to the template class typedefs for
// basic_string and basic_ostream due to QNX errors
//
// 1     12/07/00 12:59p Brian.meinke
// BEP XML Processor: XmlNode, XmlParser, XmlString, XmlException classes.
// Utilizes expat 'C'-based xml processor.
//
//******************************************************************************
#ifndef XMLDEFS_H
#define XMLDEFS_H
//==============================================================================
#include "xmlparse.h"       // expat: For XML_Char type
#include "BepException.h" 	// base exception definition
#include <sys/neutrino.h>	// For generic character string manipulation

#include <vector>
#include <map>
#include <string>

#ifdef __WATCOMC__
#include <ostream>
#elif _NTO_VERSION >= 610
#include <ostream>
#else
#include <ostream.h>
#endif

#ifdef __WATCOMC__
__STL_BEGIN_NAMESPACE
#else
using namespace std;
#endif
//==============================================================================
// Forward definition of the XmlNode class for use in typedef below
class XmlNode;
//==============================================================================
#if defined(__WATCOMC__) || defined(__QNX__)
#define DLL_CLASS
#elif defined(_DLL)
#define DLL_CLASS   _declspec( dllexport)
#else
#define DLL_CLASS   _declspec( dllimport)
#endif
//==============================================================================
#ifdef XML_UNICODE_WCHAR_T
#define XML_T(x) L ## x
typedef wstring     XmlString;
typedef wostream    XmlOStream;
#else
#define XML_T(x) x
typedef string      XmlString;
typedef ostream     XmlOStream;
#endif
//==============================================================================
#define XML_TAB     XML_T("   ")
//==============================================================================
typedef vector< XmlNode*>                                       XmlNodeList;
//==============================================================================
typedef DLL_CLASS std::map< XmlString, XmlNode*, less< XmlString> > 	XmlNodeMapType;
typedef XmlNodeMapType::iterator                                		XmlNodeMapItr;
typedef XmlNodeMapType::const_iterator                          		XmlNodeMapCItr;
typedef XmlNodeMapType::value_type                              		XmlNodeMapVal;
//==============================================================================
#endif  // XMLDEFS_H
