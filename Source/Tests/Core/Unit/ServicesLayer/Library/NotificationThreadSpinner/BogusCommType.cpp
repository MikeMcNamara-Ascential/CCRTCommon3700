// used for testing NotificationThreadSpinner

#include "BogusCommType.h"

// global stuff
std::list<std::string> BogusCommLog::myLog;
UINT32 BogusCommType::numberAlive;
UINT32 BogusCommType::numberActive;

void BogusCommLog::clearLog(void)
{
   BogusCommLog::myLog.clear();
   BogusCommLog::myLog.push_back(std::string("Log Cleared."));
}


// BogusCommType implementations

BogusCommType::BogusCommType()
{
   char locChars[50];
   numberAlive++;
   std::string locString = "Constructor - count now: ";
   sprintf(locChars, "%d", numberAlive);
   locString += locChars;
   m_BCL.myLog.push_back(locString);
}

BogusCommType::~BogusCommType()
{
   char locChars[50];
   numberAlive--;
   std::string locString = m_name;
   locString += ": Destructor called - count now: ";
   sprintf(locChars, "%d", numberAlive);
   locString += locChars;
   m_BCL.myLog.push_back(locString);

}

INT32 BogusCommType::Publish(const std::string &tag,
      const std::string &value,
      std::string &response,
      const bool IsFinal) /* = false */
{
   XmlNode temp(tag,value);
   return(Publish(&temp, response, IsFinal));
}

INT32 BogusCommType::Publish(const XmlNode *locNode, std::string &response, const bool IsFinal /*= false*/)
{

   numberActive++;
   std::string locString = "Name: ";
   locString += m_name;
   locString += ". Node Published: \"";
   locString += locNode->ToString();
   locString += "\"";

   m_BCL.myLog.push_back(locString);
   if("DelayComm1" == m_name) BposSleep(1000);
   if("DelayComm10" == m_name) BposSleep(10000);
   numberActive--;
   return(0);
}

void BogusCommType::Initialize(const std::string &name, const std::string &type,
      const bool debug /*= false*/,
      const INT32 size /*= DEFAULT_BUFFER_SIZE*/,
      const INT32 timeout /*= DEFAULT_TIMEOUT*/)
{

   m_name = name;
   m_type = type;
   m_debugLevel = debug;
   m_size = size;
   m_timeout = timeout;
   m_active = true;

   std::string locString = m_name;
   locString += ": Initialized.";
   m_BCL.myLog.push_back(locString);
}

void BogusCommType::Close(void) const
{
   // because we are a const method, we cant modify anything in our
   // class structure. fuck that
   *(bool *)&m_active = false;
}

void BogusCommType::ClearLog(void)
{
   m_BCL.myLog.clear();
}

inline const std::string BogusCommType::GetName(void)
{
   return(m_name);
}
inline std::string BogusCommType::GetType(void)
{
   return(m_type);
}
inline bool BogusCommType::GetDebug(void)
{
   return(m_debugLevel);
}
inline INT32 BogusCommType::GetSize(void)
{
   return(m_size);
}
inline INT32 BogusCommType::GetTimeout(void)
{
   return(m_timeout);
}
inline INT32 BogusCommType::GetActiveState(void)
{
   return(m_active);
}

INT32 BogusCommType::GetId(void)
{
   return(13);
}





