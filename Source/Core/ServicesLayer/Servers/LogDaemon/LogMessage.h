#ifndef _LOGMESSAGE_H_INCLUDED
#define _LOGMESSAGE_H_INCLUDED

#include "BepDefs.h"
#include "LogDefs.h"

#include <string>


/** 
 * LogMessage encapsulates a single message to be logged. 
 */
class LogMessage
{
	friend class LogMessageTest;
	public:
		LogMessage ();
		LogMessage (const UINT32 command, const std::string &timestamp, 
				const std::string &who, const std::string &message, 
				const std::string &file);
		LogMessage (const LogMessage &);

		~LogMessage ();

		const UINT32& GetCommand () const;
		const std::string& GetTimestamp () const;
		const std::string& GetWho () const;
		const std::string& GetMessage () const;
		const std::string& GetFile () const;
		const INT8& GetDelimiter () const;

		void SetCommand (const UINT32 command);
		void SetTimestamp (const std::string &timestamp);
		void SetWho (const std::string &who);
		void SetMessage (const std::string &message);
		void SetFile (const std::string &file);

	private:
		UINT32 _command;
		std::string _timestamp;
		std::string _who;
		std::string _message;
		std::string _file;
		INT8 _delimiter;
};

#endif
