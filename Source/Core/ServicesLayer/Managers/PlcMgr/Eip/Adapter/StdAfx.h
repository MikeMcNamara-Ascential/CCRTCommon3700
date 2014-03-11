// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BF593C4C_4DF9_454F_970C_D184C165C00F__INCLUDED_)
#define AFX_STDAFX_H__BF593C4C_4DF9_454F_970C_D184C165C00F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/* remove "nonstandard extension used : nameless struct/union" warning */
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop ) 

#ifdef _WIN32_WCE
#include <winsock.h>
#elif defined( WIN32 )
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#if !defined( _WIN32_WCE )
#include <direct.h>
#include <io.h>
#include <sys/timeb.h>
#include <time.h>
#endif // #if !defined( _WIN32_WCE )

#include "..\eipinc.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BF593C4C_4DF9_454F_970C_D184C165C00F__INCLUDED_)
