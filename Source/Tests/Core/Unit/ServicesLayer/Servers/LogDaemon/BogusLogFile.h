#ifndef __BOGUSLOGFILE_H
#define __BOGUSLOGFILE_H

#include "LogFile.h"

class BogusLogFile : public LogFile
{
	public:
		BogusLogFile(const string &name = "bogilog");
		virtual ~BogusLogFile();

		virtual int RotateIfNeeded();

		virtual bool wasRotated () const;
		virtual void resetRotated ();
		virtual int WriteLine(std::string);
		virtual std::string GetLine() const;	
	private:
		bool m_wasRotated;
		std::string m_lineToWrite;
};

#endif
