//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/tchar.h $
// $Author: Cward $
//   $Date: 12/21/03 6:27p $
//
// Description:
//    Generic Character Manipulation definitions (UNICODE vs ASCII)
//
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
//     $Log: /Core/ServicesLayer/Include/tchar.h $
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
// 1     6/12/03 4:25p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:10a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
// 
// 5     6/19/02 5:58p Bmeinke
// Added more function mapping defines
// 
// 4     8/28/01 10:40p Brian.meinke
//
// 3     4/25/01 5:12p Brian.meinke
// Removed duplicate definitions for _tscanf (my bad)
//
// 2     4/25/01 3:46p Brian.meinke
// Added printf family of functions
//
// 1     4/10/01 10:46a Admin
//
//
//******************************************************************************

#include <stdio.h>
#include <wchar.h>


#ifdef UNICODE

#define	_tfgetc		fgetwc 
#define	_tsgets		fgetws 
#define	_tfputc 	fputwc 
#define	_tfputs 	fputws
#define	_tfprintf 	fwprintf
#define	_tfscanf 	fwscanf
#define	_tgetc 		getwc
#define	_tgetchar 	getwchar 
#define	_tisalnum 	iswalnum 
#define	_tisalpha 	iswalpha 
#define	_tiscntrl 	iswcntrl 
#define	_tisctype 	iswctype 
#define	_tisdigit 	iswdigit 
#define	_tisgraph 	iswgraph 
#define	_tislower 	iswlower 
#define	_tisprint 	iswprint 
#define	_tispunct 	iswpunct 
#define	_tisspace 	iswspace 
#define	_tisupper 	iswupper 
#define	_tisxdigit 	iswxdigit 
#define	_tputc 		putwc 
#define	_tputchar 	putwchar 
#define	_tsprintf 	swprintf 
#define	_tsscanf 	swscanf 
#define	_ttoctrans 	towctrans 
#define	_ttolower 	towlower 
#define	_ttoupper 	towupper 
#define	_tungetc 	ungetwc 
#define	_tvfprintf 	vfwprintf 
#define	_tvfscanf 	vfwscanf 
#define	_tvsprintf 	vswprintf 
#define	_tvsscanf 	vswscanf 
#define	_tvprintf 	vwprintf 
#define	_tvscanf 	vwscanf 
#define	_tcscat 	wcscat 
#define	_tcschr 	wcschr 
#define	_tcscmp 	wcscmp 
#define	_tcscoll 	wcscoll 
#define	_tcscpy 	wcscpy 
#define	_tcscspn 	wcscspn 
#define	_tcsftime 	wcsftime 
#define	_tcslen 	wcslen 
#define	_tcsncat 	wcsncat 
#define	_tcsncmp 	wcsncmp 
#define	_tcsncpy 	wcsncpy 
#define	_tcspbrk 	wcspbrk 
#define	_tcsrchr 	wcsrchr 
#define	_tcsspn 	wcsspn 
#define	_tcsstr 	wcsstr 
#define	_tcstod 	wcstod
#define	_tcstof 	wcstof
#define	_tcstold 	wcstold 
#define	_tcstoimax 	wcstoimax
#define	_tcstoumax 	wcstoumax 
#define	_tcstok 	wcstok 
#define	_tcstol 	wcstol
#define	_tcstoll 	wcstoll 
#define	_tcstoul 	wcstoul
#define	_tcstoull 	wcstoull 
#define	_tcsxfrm 	wcsxfrm 
#define	_tmemchr 	wmemchr 
#define	_tmemcmp 	wmemcmp 
#define	_tmemcpy 	wmemcpy 
#define	_tmemmove 	wmemmove 
#define	_tmemset 	wmemset 
#define	_tprintf 	wprintf 
#define	_tscanf 	wscanf 

#else

#define	_tfgetc		fgetc 
#define	_tsgets		fgets 
#define	_tfputc 	fputc 
#define	_tfputs 	fputs
#define	_tfprintf 	fprintf
#define	_tfscanf 	fscanf
#define	_tgetc 		getc
#define	_tgetchar 	getchar 
#define	_tisalnum 	isalnum 
#define	_tisalpha 	isalpha 
#define	_tiscntrl 	iscntrl 
#define	_tisctype 	isctype 
#define	_tisdigit 	isdigit 
#define	_tisgraph 	isgraph 
#define	_tislower 	islower 
#define	_tisprint 	isprint 
#define	_tispunct 	ispunct 
#define	_tisspace 	isspace 
#define	_tisupper 	isupper 
#define	_tisxdigit 	isxdigit 
#define	_tputc 		putc 
#define	_tputchar 	putchar 
#define	_tsprintf 	sprintf 
#define	_tsscanf 	sscanf 
#define	_ttoctrans 	toctrans 
#define	_ttolower 	tolower 
#define	_ttoupper 	toupper 
#define	_tungetc 	ungetc 
#define	_tvfprintf 	vfprintf 
#define	_tvfscanf 	vfscanf 
#define	_tvsprintf 	vsprintf 
#define	_tvsscanf 	vsscanf 
#define	_tvprintf 	vprintf 
#define	_tvscanf 	vscanf 
#define	_tcscat 	strcat 
#define	_tcschr 	strchr 
#define	_tcscmp 	strcmp 
#define	_tcscoll 	strcoll 
#define	_tcscpy 	strcpy 
#define	_tcscspn 	strcspn 
#define	_tcsftime 	strftime 
#define	_tcslen 	strlen 
#define	_tcsncat 	strncat 
#define	_tcsncmp 	strncmp 
#define	_tcsncpy 	strncpy 
#define	_tcspbrk 	strpbrk 
#define	_tcsrchr 	strrchr 
#define	_tcsspn 	strspn 
#define	_tcsstr 	strstr 
#define	_tcstod 	strtod
#define	_tcstof 	strtof
#define	_tcstold 	strtold 
#define	_tcstoimax 	strtoimax
#define	_tcstoumax 	strtoumax 
#define	_tcstok 	strtok 
#define	_tcstol 	strtol
#define	_tcstoll 	strtoll 
#define	_tcstoul 	strtoul
#define	_tcstoull 	strtoull 
#define	_tcsxfrm 	strxfrm 
#define	_tmemchr 	memchr 
#define	_tmemcmp 	memcmp 
#define	_tmemcpy 	memcpy 
#define	_tmemmove 	memmove 
#define	_tmemset 	memset 
#define	_tprintf 	printf 
#define	_tscanf 	scanf 

#endif

