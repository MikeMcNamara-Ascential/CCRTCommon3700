#include <iostream>

#include "LogMsgQueueTest.h"
#include "LogMsgQueue.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

LogMsgQueueTest::LogMsgQueueTest()
{
}

LogMsgQueueTest::~LogMsgQueueTest()
{
}

void LogMsgQueueTest::setUp()
{
}

void LogMsgQueueTest::tearDown()
{
}

/**
 * See that the constructor for LogMsgQueue sets the ivars to the
 * expected default values.
 */
void LogMsgQueueTest::TestConstructor()
{
    LogMsgQueue q(25);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("maxSize wrong", (unsigned int)25, q.maxSize);

    int type;
    pthread_mutexattr_gettype(&q.lockAtts, &type);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("mutex type not set properly", PTHREAD_MUTEX_ERRORCHECK, type);
}

/**
 * See that LogMsgQueue_t represents a functioning queue structure
 * for LogMessage pointers.
 */
void LogMsgQueueTest::TestLogMsgQueue_t()
{
    LogMsgQueue_t qt;

    LogMessage msgA(0, "time1", "who1", "message1", "file1");
    LogMessage msgB(1, "time2", "who2", "message2", "file2");

    qt.push(&msgA);
    qt.push(&msgB);

    // Check for msgA
    LogMessage* ptrA = qt.front();
    qt.pop();
    LogMessage* ptrB = qt.front();
    qt.pop();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("wanted msgA", &msgA, ptrA);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("wanted msgB", &msgB, ptrB);
    
}

/**
 * See that Enqueue() and Deqeue() function as you'd expect.
 */
void LogMsgQueueTest::TestEnqueueDequeue()
{
    LogMsgQueue q(10);

    LogMessage msgA(0, "time1", "who1", "message1", "file1");
    LogMessage msgB(1, "time2", "who2", "message2", "file2");

    q.Enqueue(&msgA);
    q.Enqueue(&msgB);

    LogMessage* ptrA = q.Dequeue();
    LogMessage* ptrB = q.Dequeue();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("wanted msgA", &msgA, ptrA);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("wanted msgB", &msgB, ptrB);

}

/******************************************************************************/
/* BEGIN TESTDEQUEUE_BLOCK ****************************************************/

/** Structure to contain all the data needed for the helper threads used by the
 * TestDequeue_Block() test. */
struct TestDequeueData_t
{
    TestDequeueData_t (LogMsgQueue *q, pthread_t *con, pthread_t *prod)
    {
        queue = q;

        consumerstarted = consumerfinished = success = false;

        error = "";

        message = NULL;
        
        testover = false;
        
        consumer = con;
        producer = prod;
    }

    /** A pointer to the queue object being tested */
    LogMsgQueue *queue;

    LogMessage *message;

    pthread_t *consumer;
    pthread_t *producer;
    
    /** True if the consumer thread has tried to start to consume messages off
     * of the queue */
    bool consumerstarted;

    /** True if the consumer thread has successfully consumed its messages out
     * of the queue */
    bool consumerfinished;

    bool success;
    string error;
    bool testover;
};

/**
 * See that trying to Deqeue() on an empty queue blocks
 * and waits for content.
 */
    void LogMsgQueueTest::TestDequeue_Block()
{
    LogMsgQueue *q = new LogMsgQueue(10);

    LogMessage message;

    pthread_t consumer;
    pthread_t producer;

    TestDequeueData_t data(q, &consumer, &producer);
    data.message = &message;
    
    pthread_create (&consumer, NULL, TestDequeue_Block_Consumer, &data);
    pthread_create (&producer, NULL, TestDequeue_Block_Producer, &data);

    while (data.testover == false) {
        //waiting for tests to finish
        sleep(1);
    }

    CPPUNIT_ASSERT_MESSAGE(data.error, data.success == true);
    
    // Delete the log message queue
    delete(q);
}

/** A helper method to handle the case that something breaks */
static void abortProducer(std::string err, TestDequeueData_t *testdata) {
    testdata->error = err;
    pthread_cancel(*testdata->consumer);
    pthread_join(*testdata->consumer, NULL);
    testdata->success = false;
    testdata->testover = true;
    pthread_exit(0);
}

/** A helper method to handle the case that something breaks */
static void abortConsumer(std::string err, TestDequeueData_t *testdata) {
    testdata->error = err;
    pthread_cancel(*testdata->producer);
    pthread_join(*testdata->producer, NULL);
    testdata->success = false;
    testdata->testover = true;
    pthread_exit(0);
}

void *LogMsgQueueTest::TestDequeue_Block_Consumer (void *x)
{
    TestDequeueData_t *testdata = (TestDequeueData_t*)(x);

    /* consumer */
    // set started to true
    testdata->consumerstarted = true;

    // try to dequeue (block)
    LogMessage *message = testdata->queue->Dequeue();

    // ensure that correct message was gotten from the queue
    if( message != testdata->message) {
        abortConsumer("Dequeued incorrect message", testdata);
    }
    
    // set consumerfinished to true
    testdata->consumerfinished = true;
    // die
    pthread_exit(0);

    return NULL; // should never be reached
}

void *LogMsgQueueTest::TestDequeue_Block_Producer (void *x)
{
    TestDequeueData_t *testdata = (TestDequeueData_t*)(x);

    /* producer */
    // wait a few seconds
    sleep(3);

    // check that consumer is blocked (check var)
    if (! testdata->consumerstarted) {
        abortProducer("Consumer not started", testdata);
    }
    // ensure that queue state is unchanged
    if (! testdata->queue->queue.empty()) {
        abortProducer("Queue is not empty", testdata);
    }
    // add a message to the queue
    testdata->queue->Enqueue(testdata->message);
    // sleep a second
    sleep(1);
    // check variable to ensure that the child dequeued successfully
    if (! testdata->consumerfinished) {
        abortProducer("Consumer did not consume", testdata);
    }
    // ensure that the queue is empty again
    if (! testdata->queue->queue.empty()) {
        abortProducer("Queue is not empty", testdata);
    }
    // die
    pthread_join(*testdata->consumer, NULL);
    testdata->success = true;
    testdata->testover = true;
    pthread_exit(0);

    return NULL; // should never be reached
}

/* END TESTDEQUEUE_BLOCK ******************************************************/
/******************************************************************************/
/* BEGIN TESTENQUEUE_BLOCK ****************************************************/

/** Structure to contain all the data needed for the helper threads used by the
 * TestEnqueue_Block() test. */
struct TestEnqueueData_t
{
    TestEnqueueData_t (LogMsgQueue *q, pthread_t *check, pthread_t *prod)
    {
        queue = q;

        producerstarted = producerfinished = success = false;

        error = "";

        testover = false;
        
        checker = check;
        producer = prod;

        messageCount = 0;
    }

    /** A pointer to the queue object being tested */
    LogMsgQueue *queue;

    int messageCount;

    pthread_t *checker;
    pthread_t *producer;
    
    /** True if the producer thread has tried to start to add messages to the
     * queue */
    bool producerstarted;

    /** True if the consumer thread has successfully added its messages to the
     * end of the queue */
    bool producerfinished;

    bool success;
    string error;
    bool testover;
};

/**
 * See that trying to Enqueue() on a full queue will not block
 */
void LogMsgQueueTest::TestEnqueue_NotBlock()
{
    LogMsgQueue q(10);

    pthread_t checker;
    pthread_t producer;

    TestEnqueueData_t data(&q, &checker, &producer);

    // Fill up the message queue
    LogMessage tmpmessage;
    for (int i=0; i<10; i++)
    {
        q.Enqueue(&tmpmessage);
    }
    
    pthread_create (&producer, NULL, TestEnqueue_Block_Producer, &data);
    pthread_create (&checker, NULL, TestEnqueue_Block_Checker, &data);

    while (data.testover == false) {
        //waiting for tests to finish
        sleep(1);
    }

    CPPUNIT_ASSERT_MESSAGE(data.error, data.success == true);
}

/** A helper method to handle the case that something breaks */
static void abortProducer(std::string err, TestEnqueueData_t *testdata) {
    testdata->error = err;
    pthread_cancel(*testdata->checker);
    pthread_join(*testdata->checker, NULL);
    testdata->success = false;
    testdata->testover = true;
    pthread_exit(0);
}

/** A helper method to handle the case that something breaks */
static void abortChecker(std::string err, TestEnqueueData_t *testdata) {
    testdata->error = err;
    pthread_cancel(*testdata->producer);
    pthread_join(*testdata->producer, NULL);
    testdata->success = false;
    testdata->testover = true;
    pthread_exit(0);
}

/** Producer thread: This thread attempts to place twenty messages in a full
 * queue. It exits after that. This operation is watched by the 'checker' to
 * ensure that it happens as it should (i.e. it shouldn't get stuck on one
 * message and block) */
void *LogMsgQueueTest::TestEnqueue_Block_Producer (void *x)
{
    TestEnqueueData_t *testdata = (TestEnqueueData_t*)(x);
    if (testdata == NULL) {
        abortProducer("message was null", testdata);
    }

    // set started to true
    testdata->producerstarted = true;

    while (testdata->messageCount < 7) {
        LogMessage *m = new LogMessage;
        testdata->queue->Enqueue(m);
        testdata->messageCount++;
        sleep (1);
    }
    
    // set consumerfinished to true
    testdata->producerfinished = true;
    // die
    pthread_exit(0);

    return NULL; // should never be reached
}

/** The checker thread: this thread wakes every few seconds and makes sure that
 * the producer thread has done something (and therefore has not blocked) */
void *LogMsgQueueTest::TestEnqueue_Block_Checker (void *x)
{
    TestEnqueueData_t *testdata = (TestEnqueueData_t*)(x);

    /* producer */
    // wait a few seconds
    sleep(3);

    // check that consumer is blocked (check var)
    if (! testdata->producerstarted || testdata->producerfinished) {
        abortChecker("Producer not started", testdata);
    }
    // ensure that queue state is unchanged
    if (testdata->queue->queue.size() < 10) {
        abortChecker("Queue is not full",
                testdata);
    }

    // we will poll the data this many times to make sure it is changing as
    // expected
    int moreToCheck = 3;
    // This is the count of how many attempts to enqueue have been made by the
    // producer. This number should be increase every time this loop runs (by
    // being incremented in the producer thread)
    int oldCount = -1;
    while (moreToCheck)
    {
        int newCount = testdata->messageCount;

        if (oldCount >= newCount) {
            abortChecker("Enqueue attempt count not increasing! "
                    "(this could occur if the system was under load)",
                    testdata);
        }

        oldCount = newCount;
        
        // Checked once more
        moreToCheck--;
        // Wait some before we check again
        sleep (2);
    }

    // check variable to ensure that the child dequeued successfully
    if (! testdata->producerfinished) {
        abortChecker("Producer did not complete as expected", testdata);
    }

    // ensure that the queue is full again
    if (testdata->queue->queue.size() < 10) {
        abortChecker("Queue is not full", testdata);
    }
    // die
    pthread_join(*testdata->producer, NULL);
    testdata->success = true;
    testdata->testover = true;
    pthread_exit(0);

    return NULL; // should never be reached
}

/* END TESTENQUEUE_BLOCK ******************************************************/
/******************************************************************************/


/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
