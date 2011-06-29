//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogMessage.cpp $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		LogMessage.cpp
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogMessage.cpp 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogMessage.cpp $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.5  2003/07/16 20:42:13  crosby
//    using const ref return types form Get*
//
//    Revision 1.4  2003/07/14 19:26:18  crosby
//    Inserted doc header at top of file
//
//
//******************************************************************************
#include <string>

#include "BepDefs.h"
#include "LogMessage.h"


/** 
 * Default constructor. Sets the command field to zero (0), and leaves all
 * other fields as empty strings. 
 */
LogMessage::LogMessage ()
	: _command (0)
{
}

/** 
 * Create a logmessage with the specified command, timestamp, logger (who),
 * message, and destined for the specified file 
 */
LogMessage::LogMessage (const UINT32 command, const std::string &timestamp, const
		std::string &who, const std::string &message, const std::string &file)
	: _command (command),
	_timestamp (timestamp),
	_who (who),
	_message (message),
	_file (file)
{
	// Not allowing configuration of delimiter at this time.
	// Current log view tools are not flexible enough to dynamically 
	// choose which delimiter to use.
	_delimiter = DELIMITER_ID; // 0x80, defined in LogDefs.h
}

/** 
 * Copy constructor 
 */
LogMessage::LogMessage (const LogMessage &logmsg)
	: _command (logmsg._command),
	_timestamp (logmsg._timestamp),
	_who (logmsg._who),
	_message (logmsg._message),
	_file (logmsg._file)
{
}

/** Destructor */
LogMessage::~LogMessage ()
{
}

const UINT32& LogMessage::GetCommand () const
{
	return _command;
}

const std::string& LogMessage::GetTimestamp () const
{
	return _timestamp;
}

const std::string& LogMessage::GetWho () const
{
	return _who;
}

const std::string& LogMessage::GetMessage () const
{
	return _message;
}

const std::string& LogMessage::GetFile () const
{
	return _file;
}

const INT8& LogMessage::GetDelimiter () const
{
	return _delimiter;
}

void LogMessage::SetCommand (const UINT32 command)
{
	_command = command;
}

void LogMessage::SetTimestamp (const std::string &timestamp)
{
	_timestamp = timestamp;
}

void LogMessage::SetWho (const std::string &who)
{
	_who = who;
}

void LogMessage::SetMessage (const std::string &message)
{
	_message = message;
}

void LogMessage::SetFile (const std::string &file)
{
	_file = file;
}
