//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTest.cpp 2     2/15/06 8:08p Cward $
//
// FILE DESCRIPTION:
//   This is the test that will excersize the notification thread spinner
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTest.cpp $
// 
// 2     2/15/06 8:08p Cward
// Simplified Unit Test.
//
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     5/31/05 8:44a Gswope
// Initial entry into source safe
//
//
//*************************************************************************
#include "NotificationThreadSpinnerTest.h"
#include "NotificationThreadSpinner.h"

#define tDEBUG true


NotificationThreadSpinnerTest::NotificationThreadSpinnerTest()
{
}

NotificationThreadSpinnerTest::~NotificationThreadSpinnerTest()
{
//   if(tDEBUG)
//      printf("NotificationThreadSpinnerTest::~NotificationThreadSpinnerTest done\n");

}

/**
 * most of this method is an artifact of the standard framework.
 */
void NotificationThreadSpinnerTest::setUp()
{
   BogusCommLog::myLog.push_back(std::string("*setup()"));
   // Set up the path's we'll neeed
//   if(getenv("TEST_DIR_PATH") == NULL)
//   {
//      cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
//      exit(1);
//   }

   // create the path to the configuration files
//   string topTestDir = getenv("TEST_DIR_PATH");
//   m_configDir = topTestDir + "/Unit/ServicesLayer/NotificationThreadSpinner/Configuration/";
//   if(tDEBUG)
//      printf("The new configuration directory: %s\n", m_configDir.c_str());


   // Update the FTP_ROOT environment path
//   m_ftpRoot = getenv("FTP_ROOT");
//   setenv("FTP_ROOT", topTestDir.c_str(), 1);
   // Update the USR_ROOT environment path
//   m_usrRoot = getenv("USR_ROOT");
//   setenv("USR_ROOT", topTestDir.c_str(), 1);
}

void NotificationThreadSpinnerTest::tearDown()
{
   BogusCommLog::myLog.push_back(std::string("*tearDown()"));
//   if(tDEBUG)
//      printf("\n\nSetting the FTP and USR ROOTS\n");
   // Restore the environment variables
//   setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
//   setenv("USR_ROOT", m_usrRoot.c_str(), 1);

}

/**
 * helper method
 */
void NotificationThreadSpinnerTest::ClearLog(void)
{
   BogusCommLog::clearLog();
   BogusCommType::numberAlive = 0;

}

/**
 * This is a test of the NotificationData Class which is a
 * unique component of the NotificationThreadSpinner.
 */
void NotificationThreadSpinnerTest::NotificationDataClassTest(void)
{
   XmlNode *locNode;


   BogusCommLog::myLog.push_back(std::string("*NotificationDataClassTest()"));

   //  Dynamically allocate a NotificationData Class Object named m_NDCO.
   // (Note that this class has no friends, we can only test it through the interface.)
   NotificationData *m_NDCO = new NotificationData();
   //  Create an object to act at the bogus parent for the NDCO.
   BogusNotificationType *m_INO = new BogusNotificationType();
   //  Call m_NDCO->SetNotificationData(xml data)
   locNode = new XmlNode("name1", "data1");
   m_NDCO->SetNotificationData(locNode);
   delete locNode;
   //  Call m_NDCO->GetNotificationData()
   //  TEST: Compare data received with data sent, ensure they are equal.
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME not stored in NotificationData Object",
         (std::string("name1") == m_NDCO->GetNotificationData()->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE not stored in NotificationData Object",
         (std::string("data1") == m_NDCO->GetNotificationData()->getValue()));
   //  Call m_NDCO->SetParent, to bogus parent
   m_NDCO->SetParent((INotification *) m_INO);
   //  Call destructor for m_NDCO,
   delete m_NDCO;
   m_NDCO = NULL;
   //  TEST: Verify that parent function DecrementThreadCount is called.
   CPPUNIT_ASSERT_MESSAGE("DecrementThreadCount was not called ",
         m_INO->m_DecrementCalled);
   //  Dynamically allocate a NotificationDataClassObject named m_NDCO2.
   NotificationData *m_NDCO2 = new NotificationData();
   //  Call m_NDCO2->SetParent, to bogus parent
   m_NDCO2->SetParent((INotification *) m_INO);
   //  Call m_NDCO2->SetNotificationData(xml data)
   locNode = new XmlNode("name2", "data2");
   m_NDCO2->SetNotificationData(locNode);
   delete locNode;
   m_NDCO2->SendNotification();
   //  TEST:  Verify that parent method SendNotification is called.
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME not received by client",
         (std::string("name2") == m_INO->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE not received by client",
         (std::string("data2") == m_INO->m_node->getValue()));
   //  Call destructor for m_NDCO2
   delete m_NDCO2;
   //  Call destructor for m_INO
   delete m_INO;
}

/**
 * This test is used to test the GLOBAL method NotificationThread().
 * The method is correctly global, as it is called by the operating
 * system to implement the assorted threads. This test verifies that the
 * method is functional and thread safe.
 * It uses portions of the NotificationData class previously tested.
 */
void NotificationThreadSpinnerTest::ThreadTester(void)
{
   XmlNode *locNode;


   BogusCommLog::myLog.push_back(std::string("*ThreadTester()"));
   // Dynamically allocate a NotificationDataClassObject named m_NDCO3.
   NotificationData *m_NDCO3 = new NotificationData();
   // Dynamically allocate a NotificationDataClassObject named m_NDCO4.
   NotificationData *m_NDCO4 = new NotificationData();
   // Spin up 2 bogus parent classes.
   BogusNotificationType *m_INO3 = new BogusNotificationType();
   BogusNotificationType *m_INO4 = new BogusNotificationType();
   // Call m_NDCO3->SetParent, to bogus parent
   m_NDCO3->SetParent((INotification *) m_INO3);
   // Call m_NDCO4->SetParent, to DIFFERENT bogus parent
   m_NDCO4->SetParent((INotification *) m_INO4);
   // Call m_NDCO3->SetNotificationData(xml data)
   locNode = new XmlNode("name3", "data3");
   m_NDCO3->SetNotificationData(locNode);
   delete locNode;
   // Call m_NDCO4->SetNotificationData(xml data) [different data than step g]
   locNode = new XmlNode("name4", "data4");
   m_NDCO4->SetNotificationData(locNode);
   delete locNode;

   // Call m_NDCO3->SendNotification().
   m_NDCO3->SendNotification();
   // TEST:  Verify that correct parent method SendNotification is called, with correct data.
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME lost when sent to node 3",
         (std::string("name3") == m_INO3->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE lost when sent to node 3",
         (std::string("data3") == m_INO3->m_node->getValue()));
   // Call m_NDCO4->SendNotification().
   m_NDCO4->SendNotification();
   // TEST:  Verify that correct parent method SendNotification is called, with correct data.
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME lost when sent to node 4",
         (std::string("name4") == m_INO4->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE lost when sent to node 4",
         (std::string("data4") == m_INO4->m_node->getValue()));
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME lost on node 3, when data sent to node 4",
         (std::string("name3") == m_INO3->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE lost on node 3, when data sent to node 4",
         (std::string("data3") == m_INO3->m_node->getValue()));
   locNode = new XmlNode("name5", "data5");
   m_NDCO3->SetNotificationData(locNode);
   delete locNode;
   m_NDCO3->SendNotification();
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME lost on node 4, when data sent to node 5",
         (std::string("name4") == m_INO4->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE lost on node 4, when data sent to node 5",
         (std::string("data4") == m_INO4->m_node->getValue()));
   CPPUNIT_ASSERT_MESSAGE("Notification data NAME lost on node 3 - second pass",
         (std::string("name5") == m_INO3->m_node->getName()));
   CPPUNIT_ASSERT_MESSAGE("Notification data VALUE lost on node 3 - second pass",
         (std::string("data5") == m_INO3->m_node->getValue()));
   delete m_NDCO4;
   delete m_INO3;
   delete m_INO4;
   delete m_NDCO3;
}

/**
 * Initial test of the NotificationThreadSpinner.
 * This section verifies that notifications can be added to and
 * removed from the notification list, and that various auxilliary methods
 * work properly such as GetNumberOfSubscribers() & RemoveAll()
 */
void NotificationThreadSpinnerTest::ThreadSpinnerTest(void)
{

   XmlNode *locNode;


   BogusCommLog::myLog.push_back(std::string("*ThreadSpinnerTest()"));
   // Dynamically allocate a new NotificationThreadSpinner<BogusCommunication>*  named m_NTS.
   NotificationThreadSpinner<BogusCommType> *m_NTS =
         new NotificationThreadSpinner<BogusCommType>();
   // Call m_NTS->Initialize(NULL) (no real test done here)
   m_NTS->Initialize(NULL);
   BogusCommType *m_BCT = new BogusCommType();
   m_BCT->Initialize("brown", "client");
   BogusCommType *m_BCT2 = new BogusCommType();
   m_BCT2->Initialize("orange", "client");

   // Use m_NTS->AddNotification() method to add several notifications for each of several fake clients
   //       note that the clients don't need to exist, only the names must be unique.

   // format for AddNotification is ("data tag name", "subscriber name")

   m_NTS->AddNotification("Data1", "blue");
   m_NTS->AddNotification("Data1", "red");
   m_NTS->AddNotification("Data1", "green");
   m_NTS->AddNotification("Data1", "white");
   m_NTS->AddNotification("Data1", "yellow");

   m_NTS->AddNotification("Data2", "red");
   m_NTS->AddNotification("Data2", "blue");

   m_NTS->AddNotification("Data3", "green");
   m_NTS->AddNotification("Data3", "blue");
   m_NTS->AddNotification("Data3", "white");

   m_NTS->AddNotification("Data4", "yellow");
   m_NTS->AddNotification("Data4", "blue");

   m_NTS->AddNotification("Data5", "black");
   m_NTS->AddNotification("Data6", "yellow");

   // format for AddNotification is ( "data tag name", "subscriber comm object")
   m_NTS->AddNotification("Data2", (IPublish *) m_BCT);
   m_NTS->AddNotification("Data3", (IPublish *) m_BCT);

   m_NTS->AddNotification("Data6", (IPublish *) m_BCT2);

   // OK blue = 4, red, green, white, yellow, brown = 2, black = 1

   // Use m_NTS->GetNumberOfSubscribers() to verify that the counts are correct.
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data1",
         (5 == m_NTS->m_nList.GetNumberOfSubscribers("Data1")));
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data2",
         (3 == m_NTS->m_nList.GetNumberOfSubscribers("Data2")));
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data3",
         (4 == m_NTS->m_nList.GetNumberOfSubscribers("Data3")));
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data4",
         (2 == m_NTS->m_nList.GetNumberOfSubscribers("Data4")));
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data5",
         (1 == m_NTS->m_nList.GetNumberOfSubscribers("Data5")));
   CPPUNIT_ASSERT_MESSAGE("Wrong number of subscribers for Data6",
         (2 == m_NTS->m_nList.GetNumberOfSubscribers("Data6")));

   // Probe at m_nList, to verify the entries are valid.

   // Call m_NTS->RemoveNotification both ways, ensure that entries from the
   // list are removed by checking number of subscribers
   m_NTS->RemoveNotification("Data2", "blue");
   m_NTS->RemoveNotification("Data6", (IPublish *) m_BCT2);


   CPPUNIT_ASSERT_MESSAGE("# subscribers for Data2 after remove",
         (2 == m_NTS->m_nList.GetNumberOfSubscribers("Data2")));
   CPPUNIT_ASSERT_MESSAGE("# subscribers for Data3 after remove",
         (4 == m_NTS->m_nList.GetNumberOfSubscribers("Data3")));
   CPPUNIT_ASSERT_MESSAGE("# subscribers for Data6 - should be 1",
         (1 == m_NTS->m_nList.GetNumberOfSubscribers("Data6")));

   m_NTS->RemoveNotification("Data6", "yellow");
   CPPUNIT_ASSERT_MESSAGE("# subcribers for Data6 - should be -1",
         (-1 == m_NTS->m_nList.GetNumberOfSubscribers("Data6")));


   // Call m_NTS->Notify (2 ways) with several pieces of data

   locNode = new XmlNode("Data1", "Value1");
   m_NTS->Notify(locNode);
   delete locNode;
   locNode = new XmlNode("Data2", "Value2");
   m_NTS->Notify(locNode);
   delete locNode;
   locNode = new XmlNode("Data3", "Value3");
   m_NTS->Notify(locNode);
   delete locNode;

   m_NTS->Notify("Data4","Value4");
   m_NTS->Notify("Data5","Value5");
   m_NTS->Notify("Data6","Value6");
   m_NTS->Notify("Data1","Value1b");
   m_NTS->Notify("Data2","Value2b");

   // verify that data was sent correctly
   // still to be written

   // call RemoveAll to remove all notifications
   m_NTS->RemoveAll();

   // verify that all notifications have been removed from the list


   // this one needs to be manually deleted, since it was removed from
   // the notification list, before we called RemoveAll().
   delete m_BCT2;

   // note: if it was a comm object passed in for AddNotification,
   // the comm object is deleted by the RemoveAll() function called above.
   // This seems to be stupid, since the parent potentially still has a
   // pointer to it.  Probably should be re-designed.
   //   delete m_BCT;

   delete m_NTS;
}

/**
 * This section verifies the thread spinner functionality.
 * It creates 4 subscriptions that have inherent delays, so that
 * the thread count can be seen to increase then decrease, as designed
 *
 *
 * the second half of the test verifies that the destructor waits for
 * all child threads to die before it completes destruction
 */
void NotificationThreadSpinnerTest::ThreadSpinnerTest2(void)
{
   INT32 locThreadCount, locAlive;
   std::string locString;

   BogusCommLog::myLog.push_back(std::string("*ThreadSpinnerTest2()"));
   // Dynamically allocate a new NotificationThreadSpinner<BogusCommunication>*  named m_NTS.
   NotificationThreadSpinner<BogusCommType> *m_NTS =
         new NotificationThreadSpinner<BogusCommType>();
   // Call m_NTS->Initialize(NULL) (no real test done here)
   m_NTS->Initialize(NULL);

   // instantiate 4 comm objects that have a delay in them
   BogusCommType *m_BCT3= new BogusCommType();
   m_BCT3->Initialize("DelayComm1", "client");
   BogusCommType *m_BCT4 = new BogusCommType();
   m_BCT4->Initialize("DelayComm1", "client");
   BogusCommType *m_BCT5 = new BogusCommType();
   m_BCT5->Initialize("DelayComm1", "client");
   BogusCommType *m_BCT6 = new BogusCommType();
   m_BCT6->Initialize("DelayComm1", "client");

   m_NTS->AddNotification("Data3", (IPublish *) m_BCT3);
   m_NTS->AddNotification("Data4", (IPublish *) m_BCT4);
   m_NTS->AddNotification("Data4a", (IPublish *) m_BCT5);
   m_NTS->AddNotification("Data5", (IPublish *) m_BCT6);

   // Call m_NTS->Notify in the way that causes a client delay,

   m_NTS->Notify("Data3", "Value3x");
   m_NTS->Notify("Data4a", "Value4ax");
   m_NTS->Notify("Data4", "Value4x");
   m_NTS->Notify("Data5", "Value5x");

   // Verify that the thread count increases, then decreases,
   // when it should, by using m_NTS->GetThreadCount
   INT32 locWait = 50;
   while(m_NTS->GetThreadCount() < 4)
   {
      // wait for the count to get up to 4, fail if it never gets there
      BposSleep(100);
      if(locWait > 0) locWait--;
      else break;
   }
   CPPUNIT_ASSERT_MESSAGE("Thread count did not increase to 4",
         (locWait > 1));

   BposSleep(50);
   locThreadCount = m_NTS->GetThreadCount();
   if(-1 == locThreadCount) locThreadCount = 0;
   locAlive = BogusCommType::numberAlive;
   locWait = 50;
   while(m_NTS->GetThreadCount() > 0)
   {
      BposSleep(100);
      if(locWait > 0) locWait--;
      else break;
   }
   locThreadCount = m_NTS->GetThreadCount();
   if(-1 == locThreadCount) locThreadCount = 0;

   CPPUNIT_ASSERT_MESSAGE("Thread count did not decrease to 0",
         (locWait > 1));

   m_NTS->RemoveAll();

   // part 2 - check destructor

   // Call m_NTS->Notify in the way that caused the client delay,
   BogusCommType *m_BCT7= new BogusCommType();
   m_BCT7->Initialize("DelayComm1", "client");
   BogusCommType *m_BCT8 = new BogusCommType();
   m_BCT8->Initialize("DelayComm1", "client");

   m_NTS->AddNotification("Data6", (IPublish *) m_BCT7);
   m_NTS->AddNotification("Data6", (IPublish *) m_BCT8);

   delete m_NTS; // this should not return until both threads are dead
   CPPUNIT_ASSERT_MESSAGE("NotificationThreadSpinner Terminated before threads were dead",
         (BogusCommType::numberActive == 0));


   // deleting mNTS above does NOT delete the notification objects,
   // this is contrary to what would happen if RemoveAll() is called first.
   delete m_BCT7;
   delete m_BCT8;
}

/**
 * this test verifies that the destructor will complete,
 * even if the child threads do not terminate
 */
void NotificationThreadSpinnerTest::ThreadSpinnerTest3(void)
{
   BogusCommLog::myLog.push_back(std::string("*ThreadSpinnerTest3()"));

   // Dynamically allocate a new NotificationThreadSpinner<BogusCommunication>*  named m_NTS.
   NotificationThreadSpinner<BogusCommType> *m_NTS2 =
         new NotificationThreadSpinner<BogusCommType>();
   // Call m_NTS->Initialize(NULL) (no real test done here)
   m_NTS2->Initialize(NULL);
   BogusCommType *m_BCT9= new BogusCommType();
   m_BCT9->Initialize("DelayComm1", "client");
   BogusCommType *m_BCT10 = new BogusCommType();
   m_BCT10->Initialize("DelayComm10", "client");

   m_NTS2->AddNotification("Data7", (IPublish *) m_BCT9);
   m_NTS2->AddNotification("Data7", (IPublish *) m_BCT10);
   m_NTS2->Notify("Data7", "Value7");

   delete m_NTS2; // when this returns, there should be one live thread.
   CPPUNIT_ASSERT_MESSAGE("NotificationThreadSpinner did not terminate with one thread active.",
         (BogusCommType::numberActive == 1));

   delete m_BCT9;
   delete m_BCT10;
}

/**
 * this is a utility function that will dump the
 * log if desired, for debugging purposes.
 */
void NotificationThreadSpinnerTest::LogDumper(void)
{
   std::list<std::string>::iterator logItr;
   printf("\n----------- Log: -----------------------\n");
   for(logItr=BogusCommLog::myLog.begin(); logItr!=BogusCommLog::myLog.end();logItr++)
   {
      printf("%s\n",(*logItr).c_str());

   }
}


