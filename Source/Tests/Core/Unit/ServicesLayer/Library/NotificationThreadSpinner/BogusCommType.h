// derived from IPublish !

#ifndef BOGUSCOMMTYPE_H
#define BOGUSCOMMTYPE_H

#include "BepDefs.h"
#include "Bepos.h"
#include "XmlDefs.h"
#include "IPublish.h"
#include "XmlParser.h"
#include "Debuggable.h"
#include <list>

class BogusCommLog
{
public:
   static std::list<std::string> myLog;
   static void clearLog(void);
};

class BogusCommType : public Debuggable, public IPublish
{
public:

   // these prototypes are all copied from IPubish, where they are pure virtual

   BogusCommType();
   virtual ~BogusCommType();

   virtual INT32 Publish(const std::string &tag, const std::string &value,
         std::string &response, const bool IsFinal = false);
   virtual INT32 Publish(const XmlNode *node, std::string &response, const bool IsFinal = false);
   virtual void Initialize(const std::string &name, const std::string &type,
         const bool debug = false,
         const INT32 size = DEFAULT_BUFFER_SIZE,
         const INT32 timeout = DEFAULT_TIMEOUT);
   virtual const std::string GetName(void);
   virtual void Close(void) const;

   // additional members for test monitoring

   // staic members 
   static UINT32 numberAlive;
   static UINT32 numberActive;

   // the class below contains static members.
   BogusCommLog m_BCL;

   inline std::string GetType(void);
   inline bool GetDebug(void);
   inline INT32 GetSize(void);
   inline INT32 GetTimeout(void);
   inline INT32 GetActiveState(void);
   void ClearLog(void);

   // this method is NOT in the base class, but is needed by NotificationThread
   INT32 GetId(void);

private:

   std::string m_name;
   std::string m_type;
   bool m_debugLevel;
   INT32 m_size;
   INT32 m_timeout;
   bool m_active;

};


#endif // BOGUSCOMMTYPE_H









