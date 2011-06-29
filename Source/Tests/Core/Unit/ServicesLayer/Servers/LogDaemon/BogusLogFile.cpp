#include "LogFile.h"
#include "BogusLogFile.h"

BogusLogFile::BogusLogFile (const string &name)
	: LogFile (".", name, 1, 4),
	m_wasRotated (false)
{
}

BogusLogFile::~BogusLogFile ()
{
}

int BogusLogFile::RotateIfNeeded ()
{
	m_wasRotated = true;
    return(0);
}

bool BogusLogFile::wasRotated () const
{
	return m_wasRotated;
}

void BogusLogFile::resetRotated ()
{
	m_wasRotated = false;
}

int BogusLogFile::WriteLine(std::string pLine) {
	m_lineToWrite = pLine;
	return 0;
}

std::string BogusLogFile::GetLine() const{
	return m_lineToWrite;
}
