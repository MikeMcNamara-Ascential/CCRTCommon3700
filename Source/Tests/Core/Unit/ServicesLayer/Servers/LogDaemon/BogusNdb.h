#ifndef __BOGUSNDB_H
#define __BOGUSNDB_H

#include "INamedDataBroker.h"

class BogusNdb : public INamedDataBroker
{
public:
	BogusNdb();
	virtual ~BogusNdb();

#if 0
	virtual int RotateIfNeeded();

	virtual bool wasRotated () const;
	virtual void resetRotated ();
	virtual int WriteLine(std::string);
	virtual std::string GetLine() const;	
private:
	bool m_wasRotated;
	std::string m_lineToWrite;
#endif
};

#endif
