
#include "../../eipinc.h"

#define ASSEMBLY_INPUT_INSTANCE				100
#define ASSEMBLY_OUTPUT_INSTANCE			101
#define ASSEMBLY_CONFIGURATION_INSTANCE		102
#define ASSEMBLY_INSTANCE_DATA_SIZE			200
#define ASSEMBLY_INSTANCE_CONFIG_SIZE		4

/* Application global variables */
sem_t	geipSem;

void signal_term_handler(int sig);
void fnCallback( int nEvent, int nParam1, int nParam2 );
void test_run(void);
void* EIPTask(void* pParam);

/****************************************************************************
* FUNCTION:		main
* DESCRIPTION:	Test program main function
* INPUTS:		N/A
* RETURNS:		N/A	
****************************************************************************/

int main(int argc, char* argv[])
{
	pthread_t   lThreadId;
	EtIPIdentityInfo Info;

	/* Register the callback function. */
	clientRegisterEventCallBack(&fnCallback );

	/* startup the adapter */
	clientStart();

	clientClaimAllHostIPAddress();

	if(pthread_create(&lThreadId, NULL, EIPTask, NULL) != 0)
		return -1;

	clientAddAssemblyInstance(ASSEMBLY_OUTPUT_INSTANCE, AssemblyStatic | AssemblyProducing, INVALID_OFFSET, ASSEMBLY_INSTANCE_DATA_SIZE, NULL);
	clientAddAssemblyInstance(ASSEMBLY_INPUT_INSTANCE, AssemblyStatic | AssemblyConsuming, INVALID_OFFSET, ASSEMBLY_INSTANCE_DATA_SIZE, NULL);
	clientAddAssemblyInstance(ASSEMBLY_CONFIGURATION_INSTANCE, AssemblyStatic | AssemblyConfiguration, INVALID_OFFSET, ASSEMBLY_INSTANCE_CONFIG_SIZE, NULL);

	printf("Press a key to end application\n");
	getchar();

	clientStop();

	return 0;
}

/****************************************************************************
* FUNCTION:		signal_term_handler
* DESCRIPTION:	Handles SIGTERM signals
* INPUTS:
* RETURNS:
****************************************************************************/
void signal_term_handler(int sig)
{
	sem_post(&geipSem);
} /* end of signal_term_handler */

/****************************************************************************
* FUNCTION:		EIPTask
* DESCRIPTION:	The Stack task seed routine
* INPUTS:		N/A
* RETURNS:		N/A
****************************************************************************/

void* EIPTask(void* pParam)
{
	int rc;

	/* Declare an itimer object to start a timer */
	struct itimerval itimer;

	/* Initialize the semaphore. */
	sem_init(&geipSem,0,0);

	/* timer interval is 10 msec. */
	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_usec = TIMER_INTERVAL;

	/* Time to the next timer expiration  */
	itimer.it_value.tv_sec = 0;
	itimer.it_value.tv_usec = TIMER_INTERVAL;

	/* Register a signal handler for SIGALRM */
	signal(SIGALRM, signal_term_handler);

	/* Start a timer with the  ITIMER_REAL clock */
	if (setitimer (ITIMER_REAL, &itimer, 0) == -1)
		return NULL;

	while(!gbTerminated)
	{
		rc = sem_wait(&geipSem);
		clientMainTask();
	}

	gbReallyTerminated = TRUE;
	return NULL;
}

/****************************************************************************
* FUNCTION:		fnCallback
* DESCRIPTION:	A callback function for use by the stack to notify us of 
*				various events
* INPUTS:		N/A
* RETURNS:		N/A
****************************************************************************/

void fnCallback ( INT32 nEvent, INT32 nParam1, INT32 nParam2 )
{
	int i;
	UINT8 aData[ASSEMBLY_INSTANCE_DATA_SIZE];
	UINT8* pData;

	switch ( nEvent )
	{
		case NM_REQUEST_RESPONSE_RECEIVED:
		{
			EtIPObjectResponse Response;
			
			clientGetUnconnectedResponse( nParam1, &Response );
			
			printf("Response for request %d received with Status %d, Data Size %d\n",
						nParam1, Response.bGeneralStatus, Response.iDataSize );

			if(Response.bGeneralStatus != 0)
			{
				printf("Error Text '%s'\n",Response.errorDescription);
				break;
			}

			pData = Response.responseData;
			for (i = 0; i < Response.iDataSize; i++, pData++)
				printf("0x%x ", *pData);

			printf("\n");
		}
		break;

		case NM_ASSEMBLY_NEW_INSTANCE_DATA:
			switch(nParam1)
			{
			case ASSEMBLY_INPUT_INSTANCE:
				/* Get the current data */
				clientGetAssemblyInstanceData(ASSEMBLY_INPUT_INSTANCE, aData, ASSEMBLY_INSTANCE_DATA_SIZE);
				/* Mirror it to the output */
				clientSetAssemblyInstanceData(ASSEMBLY_OUTPUT_INSTANCE, aData, ASSEMBLY_INSTANCE_DATA_SIZE);
				break;
			}
			break;			
		case NM_REQUEST_FAILED_INVALID_NETWORK_PATH:
			printf("Request %d failed - invalid network path\n");
			break;

		case NM_REQUEST_TIMED_OUT:
			printf("Test Request %d failed - timed out waiting for response\n");
			break;

		case NM_CONNECTION_ESTABLISHED:
			printf("Connection with instance %d successfully opened\n", nParam1);
			break;
		
		case NM_CONNECTION_TIMED_OUT:
			printf("Connection with instance %d timed out\n", nParam1);
			break;

		case NM_CONNECTION_CLOSED:
			printf("Connection with instance %d closed\n", nParam1);
			break;

		default:
			break;
	}
}




