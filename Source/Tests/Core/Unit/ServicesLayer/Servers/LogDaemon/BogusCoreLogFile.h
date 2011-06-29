#ifndef __BOGUSCORELOGFILE_H
#define __BOGUSCORELOGFILE_H
#include "CoreLogFile.h"

class BogusCoreLogFile : public CoreLogFile {

	public:
		int incrementFileNameCalled;
		
		BogusCoreLogFile(const std::string &pFilename, const std::string &pBaseName);
		virtual ~BogusCoreLogFile();
		//virtual void RedirectToFile(std::string);
		virtual int RotateIfNeeded();
		virtual void IncrementFileName(const std::string& suffix);
		virtual int LimitCoreLog();
};
#endif
