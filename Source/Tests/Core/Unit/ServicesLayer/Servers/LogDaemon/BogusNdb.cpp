#include "BogusNdb.h"

BogusNdb::BogusNdb ()
	: INamedDataBroker ()
{
}

BogusNdb::~BogusNdb ()
{
}

#if 0
void BogusNdb::Write ()
{
}

bool BogusNdb::wasRotated () const
{
	return m_wasRotated;
}

void BogusNdb::resetRotated ()
{
	m_wasRotated = false;
}

int BogusNdb::WriteLine(std::string pLine) {
	m_lineToWrite = pLine;
	return 0;
}

std::string BogusNdb::GetLine() const{
	return m_lineToWrite;

}
#endif
