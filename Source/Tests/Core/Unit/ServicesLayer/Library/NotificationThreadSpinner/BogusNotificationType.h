// derived from IPublish !

#ifndef BOGUSNOTIFICATIONTYPE_H
#define BOGUSNOTIFICATIONTYPE_H

#include "BepDefs.h"
#include "XmlDefs.h"
#include "INotification.h"
#include "XmlParser.h"
#include "Debuggable.h"
#include <list>


class BogusNotificationType : public Debuggable, public INotification
{
public:

   BogusNotificationType();
   virtual ~BogusNotificationType();

public:
   // these prototypes are all copied from Notification, where they are pure virtual
	virtual void Initialize(const XmlNode *document);
	virtual void AddNotification(const std::string &tag, const std::string name);
	virtual void AddNotification(const std::string &tag, const IPublish* commObj);
	virtual INT32 RemoveNotification(const std::string &tag, const std::string name);
	virtual INT32 RemoveNotification(const std::string &tag, const IPublish* commObj);
	virtual void Notify(const std::string &tag, const std::string &value);
	virtual void Notify(const XmlNode *node);
	virtual void RemoveAll(void);
	virtual const INT32 GetNumberOfSubscribers(const std::string& tag);
    virtual void SendNotification(const XmlNode *node);
    virtual INT32 DecrementThreadCount(void);

    // these members/methods added for testing
    bool m_DecrementCalled;
    XmlNode *m_node;


};


#endif // BOGUSNOTIFICATIONTYPE_H









