//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.cpp $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		LogFileIterator
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.cpp 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogFileIterator.cpp $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.3  2003/10/01 22:44:04  anderson
//    got tests to build
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.4  2003/07/14 21:29:23  crosby
//    Fixed hasNext() logic.
//    Fixed iterator dereferencing.
//
//    Revision 1.3  2003/07/14 20:03:23  karlin
//    added main
//
//    Revision 1.2  2003/07/14 20:01:10  crosby
//    Fixed types in constructor
//
//    Revision 1.1  2003/07/14 19:18:17  karlin
//    added LogFileIterator
//
//******************************************************************************

#include "LogFileIterator.h"

/**
 * Sets the iterator
 */
LogFileIterator::LogFileIterator(LogFileMap_t *logMap) {
	first = logMap->begin();
	last = logMap->end();
}

/**
 * Empty
 */
LogFileIterator::~LogFileIterator() {
}

/**
 * True if there are more LogFiles to iterate over.
 */
bool LogFileIterator::HasNext() {
	return (first != last);
}

/**
 * Gets the next LogFile.
 */
LogFile *LogFileIterator::Next() {
	return (*first++).second;
}
