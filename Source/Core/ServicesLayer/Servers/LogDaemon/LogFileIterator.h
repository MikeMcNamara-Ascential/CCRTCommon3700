//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Logging
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.h 1     12/21/03 6:41p Cward $
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 9:25a Khykin
// Updated with changes from Flatrock.
//    Revision 1.5  2003/07/25 17:07:57  swieton
//    Added friend LogFileMapTest
//
//    Revision 1.4  2003/07/23 20:09:33  crosby
//    friend class LogFileIteratorTest;
//
//    Revision 1.3  2003/07/14 21:28:46  crosby
//    Fixed some includes
//
//    Revision 1.2  2003/07/14 20:02:05  crosby
//    Fix ifndef.
//    Forward decl of LogFileMap stuff to fix circular header inclusion.
//
//    Revision 1.1  2003/07/14 19:18:17  karlin
//    added LogFileIterator
//
//******************************************************************************

#ifndef _LOG_FILE_ITERATOR_H_INCLUDED_
#define _LOG_FILE_ITERATOR_H_INCLUDED_

#include "LogFile.h"
#include <pthread.h>
#include <map>
#include <string>

typedef std::map<std::string, LogFile*> LogFileMap_t;
class LogFileMap;

/**
 * Wraps the STL iterator for a LogFileMap.
 */
class LogFileIterator {
	friend class LogFileIteratorTest;
	friend class LogFileMapTest;
	public:
	//LogFileIterator(LogFileIterator_t fileIterator)
	LogFileIterator(LogFileMap_t*);
	virtual ~LogFileIterator();
	bool HasNext();
	LogFile *Next();
	
	private:
	LogFileMap_t::iterator first;
	LogFileMap_t::iterator last;
};

#endif
