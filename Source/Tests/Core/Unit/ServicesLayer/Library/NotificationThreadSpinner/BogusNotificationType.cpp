// used for testing NotificationThreadSpinner

#include "BogusNotificationType.h"

// global stuff

// BogusNotificationType implementations

BogusNotificationType::BogusNotificationType() :    m_DecrementCalled(false), m_node(NULL)

{
}

BogusNotificationType::~BogusNotificationType()
{
    if(NULL != m_node) delete m_node;
}

void BogusNotificationType::Initialize(const XmlNode *document)
{
   (void) document;
}
void BogusNotificationType::AddNotification(const std::string &tag, const std::string name)
{
   (void) tag;
   (void) name;
}
void BogusNotificationType::AddNotification(const std::string &tag, const IPublish* commObj)
{
   (void) tag;
   (void) commObj;
}
INT32 BogusNotificationType::RemoveNotification(const std::string &tag, const std::string name)
{
   (void) tag;
   (void) name;
   return(0);
}
INT32 BogusNotificationType::RemoveNotification(const std::string &tag, const IPublish* commObj)
{
   (void) tag;
   (void) commObj;
   return(0);
}
void BogusNotificationType::Notify(const std::string &tag, const std::string &value)
{
   (void) tag;
   (void) value;
}
void BogusNotificationType::Notify(const XmlNode *node)
{
   (void) node;
}
void BogusNotificationType::RemoveAll(void)
{
}
const INT32 BogusNotificationType::GetNumberOfSubscribers(const std::string& tag)
{
   (void) tag;
   return(0);
}
void BogusNotificationType::SendNotification(const XmlNode *node)
{
   if(NULL != m_node) delete m_node;
   m_node = node->Copy();
   (void) node;
}
INT32 BogusNotificationType::DecrementThreadCount(void)
{
   m_DecrementCalled = true;
   return(0);
}






