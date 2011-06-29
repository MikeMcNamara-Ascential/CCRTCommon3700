#ifndef __BOGUSCORELOGFILE2_H
#define __BOGUSCORELOGFILE2_H
#include "CoreLogFile.h"

class BogusCoreLogFile2 : public CoreLogFile {

	public:
		int opencalled;
		int closecalled;
		int lclcalled;
		
		BogusCoreLogFile2(const std::string &pFilePath,
			const std::string &pFilename, const std::string &pBaseName);
		virtual ~BogusCoreLogFile2();
		virtual int Open();
		virtual int Close();
		virtual int LimitCoreLog();
};
#endif
