//******************************************************************************
// Description:
//  Server used to hold the test result string received from an external tester
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Library/AuxResultServer.cpp $
// 
// 10    3/09/06 1:00a Bmeinke
// Utilitize the new AuxResultFieldList m_visibility member to remove
// result fields from the test results we return to clients
// Added logic to put test result flag field values (Pass/Fail) into a
// "Result" attribute node of the associated data field
// 
// 9     3/08/06 9:35p Bmeinke
// Removed #if 0
// 
// 8     3/01/06 1:05a Bmeinke
// Use writeNode() instead of ToString() when returning results from
// Read()
// 
// 7     11/17/05 5:38p Bmeinke
// Intialize fd=-1 inside OpenResultFile() to remove a compiler warning
// 
// 6     9/22/05 4:27p Bmeinke
// Typecast m_maxResultMapSize to an int32_t inside RotateResultNodes() to
// remove compiler warning
// 
// 5     9/16/05 9:21a Bmeinke
// Added more Log() statements to Read()
// 
// 4     9/14/05 4:34p Bmeinke
// Removed the constructor that took command line argument style
// parameters
// Added m_portName to define which logical port the results will com in
// in
// Changed the Index attributes of the ResultFields to be 0 based in the
// config file
// Added an overloaded Terminate() method in order to shutdown the
// receiver thread
// Added an overloaded Register() method in order to launch the recevier
// thread
// 
// 3     9/08/05 12:42p Bmeinke
// Don't erase anything from the stream in ProcessStream (this gets done
// in the CheckForFullString method)
// 
// 2     9/08/05 11:00a Bmeinke
// Read ResultTag from config file (defines what tag we will respond to
// for client test result requests)
// Added more logging
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 3     8/22/05 2:30p Bmeinke
// No longer set default field list values in Initialize()
// Look for "ValueTag" attributes when reading in the HLA field list to
// identify pass/fail fields
// Replace spaces in VIN strings when a client requests the HLA results
// for a specific VIN
// 
// 2     8/19/05 4:36p Bmeinke
// Added methods for loading and saving the HLA results to a file
// Added ability to read HLA test result via the Read() method
// 
// 1     8/18/05 3:43p Bmeinke
// Initial check-in (not compiled)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <libgen.h>		// dirname()
#include <string.h>		// strerror()
#include <fcntl.h>		// O_RDWR
#include <errno.h>
#include <sys/stat.h>	// mkdir(), share flags

#include "AuxResultServer.h"

AuxResultServer::AuxResultServer() : BepServer(),
	m_receiverThread( NULL), m_resultMap(NULL), m_maxResultMapSize(10), 
	m_resultFileName(""), m_resultTag(""), m_portName("")
{
	EnableLogger();
	SetVerboseMask(0x1F);
}

AuxResultServer::~AuxResultServer()
{
	Log( LOG_FN_ENTRY, "Enter AuxResultServer::~AuxResultServer()\n");

	if( NULL != m_resultMap)	delete m_resultMap;

	Log( LOG_FN_ENTRY, "Exit AuxResultServer::~AuxResultServer()\n");
}

void AuxResultServer::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Initialize()\n");

	
	// Set default values
	if( NULL != document)
	{
		const XmlNode *setup = document->getChild( "Setup");

		try
		{
			BepServer::Initialize( document);
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "Error initializing BepServer: %s\n",
				 err.GetReason());
		}

		try
		{
			m_maxResultMapSize = atoi(setup->getChild("MaxResultCount")->getValue().c_str());
		}
		catch( BepException &err)
		{
			m_maxResultMapSize = 10;
		}
		Log( LOG_DEV_DATA, "Number of result to maintain=%d\n", m_maxResultMapSize);

		try
		{
			/** 
			 * Get the list of result tags:
			 * <HLAVIN Index="0"/>
			 * .
			 * .
			 * .
			 * <LeftWheelRightAngleJudgement Index="8" ValueField="LeftWheelRightAngle"/>
			 * .
			 * .
			 */
			const XmlNodeMap& resultTags = setup->getChild("ResultFields")->getChildren();

			// Make room for (at least) the number of fields defined in the config file
			m_resultTags.clear();
			m_resultTags.reserve( resultTags.size());
			m_resultTags.resize( resultTags.size());

			XmlNodeMapCItr	itr;
			for( itr=resultTags.begin(); itr!=resultTags.end(); itr++)
			{
				const XmlNode *node = itr->second;
				if( NULL != node)
				{
					uint32_t tagIdx = atoi(node->getAttribute("Index")->getValue().c_str());
					if( m_resultTags.size() <= tagIdx)
					{
						Log( LOG_DEV_DATA, "Resizing result tag list to %d for %s\n",
							 tagIdx+1, node->getName().c_str());
						m_resultTags.reserve( tagIdx+1);
						m_resultTags.resize( tagIdx+1);
					}

					AuxResultField &resultField = m_resultTags[ tagIdx];

					resultField.SetFieldTag( node->getName());
					try
					{
						// If this is a pass/fail flag field, associate it with a value field
						resultField.SetValueTag( node->getAttribute("ValueField")->getValue());
					}
					catch(BepException &err)
					{
					}

					// Set the visibility for this field
					try
					{
						bool visible = (stricmp( "private", node->getAttribute("Scope")->getValue().c_str()) != 0);
						resultField.SetVisible( visible);
					}
					catch(BepException &err)
					{
						resultField.SetVisible( true);
					}

					if(resultField.IsPassFailFlag())
					{
						Log( LOG_DEV_DATA, "Aux Flag Field %s @ index %d for result %s\n", 
							 resultField.GetFieldTag().c_str(), tagIdx, resultField.GetValueTag().c_str());
					}
					else
					{
						Log( LOG_DEV_DATA, "Aux Value Field %s @ index %d\n", 
							 resultField.GetFieldTag().c_str(), tagIdx);
					}

					if( false == resultField.IsVisible())
					{
						Log( LOG_DEV_DATA, "Field %s @ index %d is private and will not be returned to clients\n", 
							 resultField.GetFieldTag().c_str(), tagIdx);
					}
				}
				else
				{
					throw BepException( "NULL node while processing Aux result tags");
				}
			}
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "Error initializing Aux result tags: %s\n",
				 err.GetReason());
		}

		// What tag do we use to publish our results
		try
		{
			m_resultTag = setup->getChild("ResultTag")->getValue();
			Log( LOG_DEV_DATA, "Publishing results via system tag <%s>\n", m_resultTag.c_str());
		}
		catch( BepException &err)
		{
			m_resultTag = std::string( "");
			Log( LOG_ERRORS, "Error reading test result tag: %s\n",
				 err.GetReason());
		}
		
		// Where are the Aux results stored
		try
		{
			m_resultFileName = setup->getChild("ResultFile")->getValue();
			Log( LOG_DEV_DATA, "Maintaining result in file <%s>\n", m_resultFileName.c_str());
		}
		catch( BepException &err)
		{
			m_resultFileName = std::string( "");
			Log( LOG_ERRORS, "Error reading auxiliary test result file name: %s\n",
				 err.GetReason());
		}
		ReadResultFile();

		try
		{
			m_portName = setup->getChild("LogicalPortName")->getValue();
			Log( LOG_DEV_DATA, "Receiving result from port <%s>\n", m_portName.c_str());
		}
		catch( BepException &err)
		{
			throw BepException("No logical port name specified in <Setup/LogicalPortName>");
		}


		Log( LOG_DEV_DATA, "Creating result receiver thread\n");
		if( NULL != m_receiverThread)
		{
			delete m_receiverThread;
			m_receiverThread = NULL;
		}
		m_receiverThread = new AuxResultReceiverThread( this, m_portName);
		m_receiverThread->Initialize(document);
	}
	
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::Initialize()\n");
}

void AuxResultServer::Terminate(void)
{
	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Terminate()\n");

	Log( LOG_DEV_DATA, "Calling BepServer::Terminate()\n");
	BepServer::Terminate();
	Log( LOG_DEV_DATA, "BepServer::Terminate() finished\n");

	if( NULL != m_receiverThread)
	{
		Log( LOG_DEV_DATA, "Stopping result receiver thread\n");
		m_receiverThread->Stop();
		delete m_receiverThread;
		m_receiverThread = NULL;
		Log( LOG_DEV_DATA, "Result receiver thread stopped\n");

		BposSleep(120);
	}
	
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::Terminate()\n");
}

const std::string AuxResultServer::Read(const XmlNode *node, const INT32 rate)
{
	std::string retVal(BEP_UNAVAILABLE_RESPONSE);
	
	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Read()\n");

	if( node->getName() == m_resultTag)
	{
		std::string vinTag(node->getValue());
		Log( LOG_DEV_DATA, "Attempt to read <%s>,<%s>\n", node->getName().c_str(), vinTag.c_str());
		if( NULL != m_resultMap)
		{
			// If a specific VIN is being requested
			if( vinTag.empty() == false)
			{
				// Replace all spaces in the VIN with underscores
				for( std::string::iterator itr=vinTag.begin(); itr!=vinTag.end(); itr++)
				{
					if( *itr == ' ')	*itr = '_';
				}
				// Retrieve the result for the requested VIN
				vinTag.insert( 0, "VIN_");
				try
				{
					const XmlNode *vinNode = m_resultMap->getChild( vinTag);
					if( NULL != vinNode)
					{
						retVal.clear();
						vinNode->writeNode( retVal);
						Log( LOG_ERRORS, "VIN <%s> found, Data=<\n%s\n>\n", vinTag.c_str(), retVal.c_str());
					}
					else
					{
						Log( LOG_ERRORS, "VIN <%s> found in map, but no data\n", vinTag.c_str());
					}
				}
				catch( BepException &err)
				{
					Log( LOG_ERRORS, "VIN <%s> not found in map\n", vinTag.c_str());
					retVal = BEP_UNAVAILABLE_RESPONSE;
				}
			}
			// No specific VIN requested, return the entire result list
			else
			{
				retVal = m_resultMap->ToString();
			}
		}
	}
	else
	{
		retVal = BepServer::Read(node, rate);
	}

	Log( LOG_FN_ENTRY, "Exit AuxResultServer::Read()\n");
	
	return( retVal);
}

const std::string AuxResultServer::Write(const XmlNode *node)
{
	std::string retVal;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Write()\n");

	retVal = BepServer::Write(node);

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Write()\n");

	return( retVal);
}

const std::string AuxResultServer::Command(const XmlNode *node)
{
	std::string retVal;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Command()\n");

	retVal = BepServer::Command(node);

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Command()\n");

	return( retVal);
}

const std::string AuxResultServer::Publish(const XmlNode *node)
{
	std::string retVal;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Publish()\n");

	retVal = BepServer::Publish(node);

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Publish()\n");

	return( retVal);
}

const std::string AuxResultServer::Register(void)
{
	std::string retVal;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Register()\n");
		
	retVal = BepServer::Register();

	if( NULL != m_receiverThread)
	{
		Log( LOG_DEV_DATA, "Launching result receiver thread\n");
		m_receiverThread->Resume( false);
	}

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::Register()\n");

	return( retVal);
}

bool AuxResultServer::ProcessStream( std::string &resultStream)
{
	std::string				resultString;
	std::string				vinString("NoVIN");
	bool					fullStringRcvd;
	XmlElement				resultNode("VIN", "", NULL);

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::ProcessStream()\n");

	// Check if we have a full result string and copy it to 'resultString'
	//	if we do
	fullStringRcvd = CheckForFullString(resultStream, resultString);
	if( true == fullStringRcvd)
	{
		Log( LOG_DEV_DATA, "Result string appears to be complete\n");
		// Parse Aux results into an XML node
		fullStringRcvd = ParseStream( resultString, vinString, resultNode);
		if( true == fullStringRcvd)
		{
			Log( LOG_DEV_DATA, "Result string parsed\n");
			// Replace all spaces in the VIN with underscores
			for( std::string::iterator itr=vinString.begin(); itr!=vinString.end(); itr++)
			{
				if( *itr == ' ')	*itr = '_';
			}

			// Finalize the result record
			FinalizeResultRecord( &resultNode);
			
			// Publish this new result
			NewRecordReceived( &resultNode, vinString);
		}
		else
		{
			Log( LOG_ERRORS, "ERROR: Unable to parse result string\n");
		}
	}

	Log( LOG_FN_ENTRY, "Exit AuxResultServer::ProcessStream(), status=%d\n", fullStringRcvd);
	
	return( fullStringRcvd);
}

void AuxResultServer::FinalizeResultRecord( XmlNode *resultNode)
{
	AuxResultFieldListCItr_t	fieldItr=m_resultTags.begin();

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::FinalizeResultRecord()\n");

	// Copy any pass/fail fields to the "Result" attribute of the associated data fields
	while( fieldItr != m_resultTags.end())
	{
		XmlNodeMap	&resultNodes = const_cast<XmlNodeMap&>(resultNode->getChildren());
		XmlNode		*dataNode, *flagNode;

		const AuxResultField& resultField = *fieldItr;
		const std::string& flagFieldTag = resultField.GetFieldTag();
		const std::string& dataFieldTag = resultField.GetValueTag();

		Log( LOG_DEV_DATA, "Analyzing field %s\n", flagFieldTag.c_str());
		
		// Does it have a data field associated with it?
		if( (false == dataFieldTag.empty()) && (false == flagFieldTag.empty()) )
		{
			Log( LOG_DEV_DATA, "Result field %s linked to %s\n",
				 flagFieldTag.c_str(), dataFieldTag.c_str());
			try
			{
				// Can we find the flag node and its associated data field
				flagNode = (XmlNode*)resultNodes.getNode( flagFieldTag);
				dataNode = (XmlNode*)resultNodes.getNode( dataFieldTag);
				if( (NULL != dataNode) && (NULL != flagNode))
				{
					Log( LOG_DEV_DATA, "Setting result field for %s to %s\n",
						 dataFieldTag.c_str(), flagNode->getValue().c_str());
					// Add a new result attribute to the data field
					(void)dataNode->addChild( "Result", flagNode->getValue(), ATTRIBUTE_NODE);
				}
			}
			catch( XmlException &err)
			{
				Log( LOG_ERRORS, "Invalid data field (%s) or result field (%s)\n",
					 dataFieldTag.c_str(), flagFieldTag.c_str());
			}
		}
		else if( true == flagFieldTag.empty())
		{
			Log( LOG_ERRORS, "Empty field tag!!\n");
		}

		// If this field is not visible to clients
		if( false == resultField.IsVisible())
		{
			try
			{
				Log( LOG_DEV_DATA, "Removing field %s from results\n", flagFieldTag.c_str());
				// Remove it from the result node map
				resultNode->delChild( flagFieldTag.c_str());
			}
			catch( XmlException &err)
			{
				Log( LOG_ERRORS, "Unable to remove privately scoped field: %s\n", flagFieldTag.c_str());
			}
		}

		fieldItr++;
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::FinalizeResultRecord()\n");
}

void AuxResultServer::NewRecordReceived( const XmlNode *resultNode, const std::string &vinStr)
{
	XmlNode 			*newResult;
	int 				newIndex = 0;
	char				indexString[ 16];
	const std::string	vinTag("VIN_" + vinStr);

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::NewRecordReceived(%s)\n", vinStr.c_str());

	if( NULL != m_resultMap)
	{
		try
		{
			// If the current VIN already exists, re-use its index
			newIndex = atoi(m_resultMap->getChild( vinTag)->getAttribute("Index")->getValue().c_str());
			// Remove old entry for this VIN
			m_resultMap->delChild( vinTag.c_str());
		}
		catch(...)
		{
			// This is a new VIN, so rotate the indices of all the existing result nodes by +1
			RotateResultNodes();
		}

		// Make a copy of the new result node
		newResult = resultNode->Copy();
		newResult->setName( std::string("VIN_") + vinStr);
		
		// Add this result at index 0
		newResult->addChild( "Index", ultoa( newIndex, indexString, 10), ATTRIBUTE_NODE);
		m_resultMap->addChild( newResult);

		WriteResultFile();
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::NewRecordReceived(%s)\n", vinStr.c_str());
}

void AuxResultServer::RotateResultNodes()
{
	int32_t					nodeIdx;
	char					indexString[ 1024];
	
	Log( LOG_FN_ENTRY, "Enter AuxResultServer::RotateResultNodes()\n");

	if( NULL != m_resultMap)
	{
		XmlNodeMap		&results = (XmlNodeMap&)m_resultMap->getChildren();
		XmlNodeMapItr	itr=results.begin();

		// Lock the map while we shuffle its nodes
		if( EOK == results.Lock())
		{
			// Erase results that are too old
			while( itr!=results.end())
			{
				XmlNode *node=itr->second;
				try
				{
					// Get the current index of this node
					XmlNode *index = (XmlNode*)node->getAttribute("Index");
					nodeIdx = atol(index->getValue().c_str()) + 1;
					// If this node's index is valid
					if( (nodeIdx < (int32_t)m_maxResultMapSize) && (nodeIdx >= 0))
					{
						// Increment the node's index
						index->setValue( ltoa( nodeIdx, indexString, 10));
						// Process the next node
						itr++;
					}
					else
					{
						Log( LOG_DEV_DATA, "Removing result <%s> with index=%d\n", 
							 node->getName().c_str(), nodeIdx);
						// Remove the node from the list (and move to the next node)
						itr = results.erase( itr);
					}
				}
				// Invalid (or non-existent) node index
				catch(BepException &err)
				{
					Log( LOG_ERRORS, "ERROR: Invalid (or non-existent) result node index\n");
					// Remove the node from the list (and move to the next node)
					itr = results.erase( itr);
					// Free the memory for this node
					delete node;
				}
			}
			results.Unlock();
		}
	}
	else
	{
		Log( LOG_ERRORS, "ERROR: Unable to rotate results because result map is NULL\n");
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::RotateResultNodes()\n");
}

void AuxResultServer::ReadResultFile()
{
	XmlParser	resultsParser;
	int			fd;
	struct stat	statBuf;

	memset( &statBuf, 0, sizeof( statBuf));

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::ReadResultFile(%s)\n", m_resultFileName.c_str());

	try
	{
		// Open the result file
		fd = OpenResultFile();
		if( -1 != fd)
		{
			// Make sure the file exists and has something in it
			if( (-1 != fstat(fd, &statBuf)) && (statBuf.st_size > 0))
			{
				const int fileSize = statBuf.st_size;
				char xmlBuff[ fileSize+1];
				// Read the file into memory
				if( read( fd, xmlBuff, fileSize) > 0)
				{
					// Delete the existing 
					if( NULL != m_resultMap)	delete m_resultMap;
					m_resultMap = NULL;

					resultsParser.Parse( xmlBuff, fileSize, true, "ROOT");
					m_resultMap = resultsParser.getDocElement()->Copy();

					Log( LOG_DEV_DATA, "Current File:\n%s\n", xmlBuff);
				}
				else
				{
					Log( LOG_ERRORS, "Unable to read result file\n");
				}
			}
			else
			{
				Log( LOG_ERRORS, "Result file does not exist or has 0 length\n");
			}
			close( fd);
		}
		else
		{
			Log( LOG_ERRORS, "Unable to open result file\n");
		}
	}
	catch( BepException &err)
	{
	}

	// If there was an error reading the results file
	if( m_resultMap == NULL)
	{
		// Make sure we have a valid node to put result into
		m_resultMap = new XmlElement( m_resultTag, "", NULL);
	}
	else
	{
		m_resultMap->setName( m_resultTag);
	}
	
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::ReadResultFile(%s)\n", m_resultFileName.c_str());
}

void AuxResultServer::WriteResultFile()
{
	std::string		resultsString( std::string("<") + m_resultMap->getName()+ std::string(">"));
	XmlNodeMapCItr	itr;
	int				fd;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::WriteResultFile(%s)\n", m_resultFileName.c_str());

	// Put the individual result records on individual lines
	for( itr=m_resultMap->getChildren().begin(); itr!=m_resultMap->getChildren().end(); itr++)
	{
		XmlNode *node = itr->second;
		
		resultsString += "\n";
		node->writeNode( resultsString);
	}
	resultsString += std::string("\n</") + m_resultMap->getName() + std::string(">");
	
	fd = OpenResultFile(true);
	if( -1 != fd)
	{
		write( fd, resultsString.c_str(), resultsString.size());
		close( fd);
	}
	else
	{
		Log( LOG_ERRORS, "ERROR: Could not open result file %s\n", m_resultFileName.c_str());
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::WriteResultFile(%s)\n", m_resultFileName.c_str());
}

int AuxResultServer::OpenResultFile(bool recreate)
{
	const char	*ftpRoot = getenv("FTP_ROOT");
	std::string	resultsFile;
	int			mode = O_RDWR|O_CREAT;
	int		   	fd=-1;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::OpenResultFile(%s, %d)\n", 
		 m_resultFileName.c_str(), recreate);

	if( NULL == ftpRoot)	ftpRoot = "/home/CCRT/Rewrite";
	resultsFile = ftpRoot + m_resultFileName;

	if( true == recreate)	mode |= O_TRUNC;
	
	// Make sure all the requierd paths exists
	if( -1 != CreateFilePath( resultsFile))
	{
		if( -1 == (fd=open( resultsFile.c_str(), O_RDWR|O_CREAT, 0666)) )
		{
			Log( LOG_ERRORS, "Error %d opening %s: %s\n", errno, strerror( errno));
		}
		else
		{
			Log( LOG_DEV_DATA, "Result file <%s> opened\n", resultsFile.c_str());
		}
	}
	else
	{
		Log( LOG_ERRORS, "Unable to create directories for %s: %s\n", 
			 resultsFile.c_str(), strerror( errno));
	}

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::OpenResultFile(%s, %d), fd=%d\n", 
		 m_resultFileName.c_str(), recreate, fd);

	return( fd);
}

int AuxResultServer::CreateFilePath( const std::string &fileName)
{
	std::string		currDir;
	struct stat		statBuf;
	bool			createDir=true;
	int				retVal = 0;

	Log( LOG_FN_ENTRY, "Enter AuxResultServer::CreateFilePath(%s)\n", fileName.c_str());

	memset( &statBuf, 0, sizeof( statBuf));
	// Make sure the file does not exist
	if( -1 == stat( fileName.c_str(), &statBuf))
	{
		char 				*junk = strdup( fileName.c_str());
		const std::string	dirName(dirname(junk));
		uint32_t			nextSlash=dirName.find('/', 1);

		while( (nextSlash != dirName.npos) && (currDir.size() < dirName.size()) && 
			   (0 == retVal))
		{
			currDir = dirName.substr(0, nextSlash);
			// Check if this directory already exists
			if( -1 == stat( currDir.c_str(), &statBuf))		createDir = true;
			else if( false == S_ISDIR(statBuf.st_mode))		createDir = true;
			else											createDir = false;
			// Check if this directory already exists
			if( true == createDir)
			{
				retVal = mkdir( currDir.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);

				if( -1 != retVal)
				{
					Log( LOG_DEV_DATA, "Created directory %s\n", currDir.c_str());
				}
				else
				{
					Log( LOG_ERRORS, "Error creating directory %s: %s\n", 
						 currDir.c_str(), strerror( errno));
				}
			}
			// Find the next delimitter
			nextSlash = dirName.find('/', nextSlash+1);
			if( (nextSlash == dirName.npos) && (currDir.size() < dirName.size()) )
			{
				nextSlash = dirName.size();
			}
		}
		free( junk);
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultServer::CreateFilePath(%s), status=%d\n", fileName.c_str(), retVal);

	return( retVal);
}

