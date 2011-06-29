#ifndef __BOGUSLOGWRITER_H
#define __BOGUSLOGWRITER_H

#include "LogWriter.h"

class BogusLogWriter
	: public LogWriter
{
	friend class LogWriterTest;
	public:
		BogusLogWriter(LogMsgQueue*, LogFileMap*);
		virtual ~BogusLogWriter();

		enum
		{
			ACTIVATE_LOG_FILE,
			DEACTIVATE_LOG_FILE,
			PROCESS_UNKNOWN_COMMAND,
			PROCESS_NORMAL_MESSAGE,
			INCREMENT_CORE_LOG_FILE,
			NONE
		};
		virtual int getLastCalled () const;

		virtual void ProcessNormalMessage(LogMessage*);

	protected:
		virtual int ActivateLogFile(LogMessage*);
		virtual int DeActivateLogFile(LogMessage*);
		virtual int IncrementCoreLogFile(LogMessage*);
		virtual void ProcessUnknownCommand(LogMessage*);

	private:
		int _lastcalled;
};

#endif
