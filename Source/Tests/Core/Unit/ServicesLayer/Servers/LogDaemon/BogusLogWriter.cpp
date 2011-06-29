#include "BogusLogWriter.h"

BogusLogWriter::BogusLogWriter(LogMsgQueue *queue, LogFileMap *map)
	: LogWriter  (queue, map),
	_lastcalled (NONE)
{
}

BogusLogWriter::~BogusLogWriter()
{
}

int BogusLogWriter::getLastCalled() const
{
	return _lastcalled;
}

void BogusLogWriter::ProcessNormalMessage(LogMessage *mesg)
{
	_lastcalled = PROCESS_NORMAL_MESSAGE;
}

void BogusLogWriter::ProcessUnknownCommand(LogMessage *mesg)
{
	_lastcalled = PROCESS_UNKNOWN_COMMAND;
}

int BogusLogWriter::ActivateLogFile(LogMessage *mesg)
{
	_lastcalled = ACTIVATE_LOG_FILE;
    return(0);
}

int BogusLogWriter::DeActivateLogFile(LogMessage *mesg)
{
	_lastcalled = DEACTIVATE_LOG_FILE;
    return(0);
}

int BogusLogWriter::IncrementCoreLogFile(LogMessage *mesg)
{
	_lastcalled = INCREMENT_CORE_LOG_FILE;
    return(0);
}
