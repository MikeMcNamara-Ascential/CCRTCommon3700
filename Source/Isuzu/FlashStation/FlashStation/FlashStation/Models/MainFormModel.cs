using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Common.Lib.Interfaces;
using System.Threading;
using VehicleCommServer;
using InputServer;
using Common.Classes;
using System.IO;
using System.Xml;
using LoggingInterface;
using UtilityFileInterpreter;
using FileChangeMonitor;
using J2534ChannelLibrary;
using System.Drawing;

namespace Common.Lib.Models
{
    public class MainFormModel  : IMainFormModel
    {



        /// <summary>
        /// Create a new Presenter responsible for coordinating Model and View
        /// </summary>
        /// <param name="view">View form interface</param>
        /// <param name="logger">logging interface for form display and file output</param>
        public MainFormModel()
        {
            m_vehicleCommInterface = new VehicleCommServerInterface();
            m_inputServerInterface = new InputServerInterface();
            m_cableConnectForced = false;
            m_cableConnectMessage = null;
            m_usingCableConnectForced = false;
            //to do get these to program settings
            m_deviceName = "null";
            m_channelName = "null";
            m_buildData = new List<ECUBuildData>();
            m_ecuNames = new List<string>();
//TO DO     make ecu names configurable
            m_ecuNames.Add("ECM");
            m_ecuNames.Add("TCM");
            m_terminateThreads = false;
            System.Windows.Forms.RichTextBox rtb = new System.Windows.Forms.RichTextBox();
            m_ecmLogger = new Logger(rtb, "ECMLog", m_logsDirectory);
            System.Windows.Forms.RichTextBox tcmRTB = new System.Windows.Forms.RichTextBox();
            m_tcmLogger = new Logger(tcmRTB, "TCMLog", m_logsDirectory);
            m_performTCMFlash = true;
            m_performECMFlash = true;

        }


        public void CommandState(StateName state)
        {
            if (GetStatus() != Status.TERMINATE)
            {
                Prompt prompt = new Prompt();
                ThreadStart taskDelegate = null;
                m_currentState = state;
                switch (state)
                {
                    case StateName.WAIT_FOR_USER_INPUT:
                        //Wait for input for build data selection
                        SetBarcode("");
                        m_buildDataValid = false;
                        m_progressBarValue = 0;
                        m_progressBarRangeMax = 0;
                        m_progressBarRangeMin = 0;
                        SetPrompt1(prompt.WAIT_FOR_USER_INPUT1);
                        SetPrompt2(prompt.WAIT_FOR_USER_INPUT2);
                        SetPrompt1BGColor(Color.White);
                        SetPrompt2BGColor(Color.White);
                        m_tcmResultColor = Color.White;
                        m_ecmResultColor = Color.White;
                        taskDelegate = new ThreadStart(WaitForUserInput);
                        break;
                    case StateName.OBTAIN_BUILD_DATA:
                        //Start build data thread
                        SetPrompt1(prompt.OBTAIN_BUILD_DATA1);
                        SetPrompt2(prompt.OBTAIN_BUILD_DATA2);
                        taskDelegate = new ThreadStart(ObtainBuildData);
                        break;
                    case StateName.WAIT_FOR_CABLECONNECT:
                        //Start Wait for cable connect thread
                        //SetPrompt1(prompt.WAIT_FOR_CABLECONNECT1);
                        SetPrompt1(prompt.WAIT_FOR_CABLECONNECT2);
                        SetPrompt2("");
                        taskDelegate = new ThreadStart(WaitForCableConnect);
                        break;
                    case StateName.FLASH_ECUS:
                        //Start vehicle flash thread
                        SetPrompt1(prompt.FLASH_ECUS1);
                        SetPrompt2(prompt.FLASH_ECUS2);
                        taskDelegate = new ThreadStart(FlashECUs);
                        break;
                    case StateName.REPORT_DATA:
                        //Start vehicle flash thread
                        //SetPrompt1(prompt.REPORT_DATA1);
                        SetPrompt2(prompt.REPORT_DATA2);
                        taskDelegate = new ThreadStart(ReportDataState);
                        break;
                    case StateName.WAIT_FOR_CABLEDISCONNECT:
                        //Start Wait for cable connect thread
                        //prompt 1 will be set by report data
                        SetPrompt2(prompt.WAIT_FOR_CABLEDISCONNECT);
                        taskDelegate = new ThreadStart(WaitForCableDisconnect);
                        break;
                    default://not supported
                        break;
                }
                Thread taskThread = new Thread(taskDelegate);
                taskThread.Start();
            }
        }
        
        public void SetLogger(Logger logger)
        {
            m_logger = logger;
            m_fcm = new FCM(m_remoteBuildFileDirectory, m_buildFileDirectory, m_tempBuildFileDirectory, 10000, m_logger);
        }

        public Status GetStatus()
        {
            return m_status;
        }
        public void SetStatus(Status status)
        {
            m_status = status;
        }
        public string GetPrompt1()
        {
            return m_prompt1;
        }
        public void SetPrompt1(string promptText)
        {
            m_prompt1 = promptText;
        }
        public string GetPrompt2()
        {
            return m_prompt2;
        }
        public void SetPrompt2(string promptText)
        {
            m_prompt2 = promptText;
        }
        public Color GetPrompt1BGColor()
        {
            return m_prompt1BGColor;
        }
        public void SetPrompt1BGColor(Color color)
        {
            m_prompt1BGColor = color;
        }
        public Color GetPrompt2BGColor()
        {
            return m_prompt2BGColor;
        }
        public void SetPrompt2BGColor(Color color)
        {
            m_prompt2BGColor = color;
        }
        public string GetBarcode()
        {
            return m_barcode;
        }
        public void SetBarcode(string barcode)
        {
            if (m_currentState == StateName.WAIT_FOR_USER_INPUT)
            {//only allow barcode input if in user input state
                m_barcode = barcode;
            }
        }

        public void WaitForUserInput()
        {
            //start serial thread to allow barcode input
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(GetSerialDeviceInput);
            Thread taskThread = new Thread(taskDelegate);
            taskThread.Start();
            while ((GetBarcode() == "") && GetStatus() == Status.IN_PROGRESS && !m_terminateThreads)
            {
                Thread.Sleep(500);
            }
            if (GetStatus() != Status.TERMINATE) SetStatus(Status.SUCCESS);
        }
        public void GetSerialDeviceInput()
        {
            if (m_serialPort==null)
                m_serialPort = new SerialPortLibrary.CcrtSerialPort();
            m_serialPort.UpdatePortSettings(m_inputServerInterface.GetSerialDevicePortSettings(GetBarcodeInputDeviceName()));
            m_serialPort.OpenPort();
            int totalBytesRead = 0;
            int bytesRead = 0;
            string barcode="";
            byte[] interReceivedData = new byte[BARCODE_LENGTH];
            List<byte> receivedData = new List<byte>();

            while (totalBytesRead < BARCODE_LENGTH && GetBarcode() == "" && GetStatus() == Status.IN_PROGRESS && !m_terminateThreads)
            {//wait for user input or scanned data
                bytesRead = m_serialPort.ReadSerialData(interReceivedData, BARCODE_LENGTH);
                if (bytesRead > 0)
                {
                    totalBytesRead += bytesRead;
                    for (int x = 0; x < bytesRead; x++)
                    {
                        receivedData.Add(interReceivedData[x]);
                    }
                }
                Thread.Sleep(100);
            }
            //for (int x = 0; x < totalBytesRead; x++)
            //{
            //    receivedData.Add(serialPort.RxData[x]);
            //}
            if (GetStatus() == Status.IN_PROGRESS)
            {
                barcode = System.Text.Encoding.ASCII.GetString(receivedData.ToArray());
                if (totalBytesRead == 30)
                {//set barcode value
                    m_logger.Log("INFO:  Received Barcode Input: " + barcode);
                    SetBarcode(barcode);
                }
            }
            m_serialPort.StopRxDataMonitor();
            m_serialPort.ClosePort();
        }
        public void ObtainBuildData()
        {
            string barcode = GetBarcode();
            bool buildDataValid = false;
            if (barcode.Length == BARCODE_LENGTH)
            {
                string vin = barcode.Substring(0, 17);
                string modelCode = barcode.Substring(17, 5);
                string bookCode = barcode.Substring(22, 2);
                string modelYear = barcode.Substring(24, 3);
                string lotNumber = barcode.Substring(27, 3);
                if (lotNumber[0] == 'P')
                {
                    lotNumber = lotNumber.Replace("P", "0");
                }
                string fileName = "Ps" + modelCode + "_" + bookCode + "_" + modelYear + "_";
                buildDataValid = Load(m_buildFileDirectory + fileName, lotNumber, vin);
            }
            else
            {//invalid barcode length
                buildDataValid = false;
                m_logger.Log("ERROR:  Invalid Barcode Length Required: " + BARCODE_LENGTH.ToString() + " received: " + barcode.Length.ToString());
            }
            if (!buildDataValid)
            {
                Prompt prompt = new Prompt();
                SetPrompt1(prompt.ABORT_INVALID_BUILD_DATA1);
                SetPrompt2(prompt.ABORT_INVALID_BUILD_DATA2);
                SetStatus(Status.ABORT);
                m_logger.Log("ERROR:  Build data load failure");
            }
            else
            {
                SetStatus(Status.SUCCESS);
            }
            m_buildDataValid = buildDataValid;
        }
        /// <summary>
        /// Load build file
        /// </summary>
        public bool Load(string fileName, string lotNo,string vin)
        {
            bool success = true;
            m_buildData.Clear();
            //load file
            try
            {   // Load the option association file
                // find file with effective lot number
                while (!File.Exists(fileName + lotNo + ".xml") && Convert.ToInt16(lotNo) > 0)
                {
                    int lot = Convert.ToInt16(lotNo) - 1;
                    lotNo = String.Format("{0:000}", lot);
                }
                if (File.Exists(fileName + lotNo + ".xml"))
                {   // Read the options from the file
                    m_currentBuild = new XmlDocument();
                    m_currentBuild.Load(fileName + lotNo + ".xml");
                    //for each ecu...
                    foreach (string ecuName in m_ecuNames)
                    {
                        string buildFileECUName = "";
                        if (ecuName == "ECM")
                        {
                            buildFileECUName = "PCM";
                        }
                        else
                        {
                            buildFileECUName = ecuName;
                        }
                        
                        ECUBuildData ecuBuild = new ECUBuildData();
                        ecuBuild.ECUName = ecuName;
                        ecuBuild.VIN = vin;
                        string searchNode;
                        //get utility file pn
                        searchNode = buildFileECUName + "UtilityFilePartNo";
                        XmlNodeList list = m_currentBuild.GetElementsByTagName(searchNode);
                        ecuBuild.UtilityFilePartNumber = list[0].InnerText;
                        //get partnum
                        if (ecuName == "TCM")
                        {
                            searchNode = buildFileECUName + "ASM" + "PartNo";
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x07);
                            ecuBuild.RequestID.Add(0xe2);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x07);
                            ecuBuild.ResponseID.Add(0xeA);
                            ecuBuild.PerformFlash = m_performTCMFlash;
                        }
                        else
                        {
                            searchNode = buildFileECUName + "PartNo";
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x07);
                            ecuBuild.RequestID.Add(0xe0);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x07);
                            ecuBuild.ResponseID.Add(0xe8);
                            ecuBuild.PerformFlash = m_performECMFlash;
                        }
                        ecuBuild.PartNumber = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                        //get sofware module part pns
                        for (int x = 0; x < MAX_NUM_SOFTWARE_PART_NOS; x++)
                        {
                            searchNode = buildFileECUName + "SoftwareModule" + String.Format("{0:00}", x) + "PartNo";
                            try
                            {
                                ecuBuild.SoftwareModulePartNumbers.Add(m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText);
                            }
                            catch
                            {//nothing special - pn might not exist
                            }
                        }
                        
                        m_buildData.Add(ecuBuild);
                    }
                }
                else
                {//error
                    success = false;
                    m_logger.Log("ERROR:  No matching file found for file prefix: " + fileName);

                }
            }
            catch
            {//error
                success = false;
                m_logger.Log("ERROR:  Exception loading file: " + fileName + lotNo + ".xml");
                
            }
            return success;
            
        }
        public void WaitForCableConnect()
        {
            while (!IsCableConnected() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
            {
                Thread.Sleep(500);
            }
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT) 
                SetStatus(Status.SUCCESS);
        }
        public void WaitForCableDisconnect()
        {
            while (IsCableConnected() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
            {
                Thread.Sleep(500);
            }
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT)
                SetStatus(Status.SUCCESS);
        }

        string m_resultText;
        public void FlashECUs()
        {
                                //Repeatability Test
                    //int iteration = 0;
                    //m_buildData[0].ProgrammingSuccess = true;
                    //m_buildData[1].ProgrammingSuccess = true;
                    //while ((GetStatus() == Status.IN_PROGRESS) && m_buildData[0].ProgrammingSuccess && m_buildData[1].ProgrammingSuccess)
                    //{
                        //iteration++;
                        //m_progressBarValue = 0;
                        //m_logger.Log("INFO:  Starting Flash Process Iteration: " + iteration.ToString());
                        //end repeatability test add
                        Status preFileProcessStatus = Status.FAILURE;
                        m_vehicleCommInterface.ClearResponseBuffer(m_deviceName, m_channelName);
                        preFileProcessStatus = PreUtilityFileProcess();
                        OpenTCMInterpreter();
                        OpenECMInterpreter();
                        m_resultText = "";
                        m_ecmThreadComplete = false;
                        m_tcmThreadComplete = false;
                        //start progress update thread
                        m_stopProgressBarThread = false;
                        ThreadStart taskDelegatePB = null;
                        taskDelegatePB = new ThreadStart(ProgressUpdateThread);
                        Thread taskThreadPB = new Thread(taskDelegatePB);
                        taskThreadPB.Start();

                        if (preFileProcessStatus == Status.SUCCESS)
                        {
                            if (m_performECMFlash)
                            {
                                //for each ecu start a utility file interpreter thread
                                ThreadStart taskDelegate = null;
                                taskDelegate = new ThreadStart(SequenceECMInterpreter);
                                Thread taskThread = new Thread(taskDelegate);
                                taskThread.Start();


                                //while not complete....
                                while (!m_ecmThreadComplete && GetStatus() == Status.IN_PROGRESS && !m_terminateThreads)
                                {
                                    Thread.Sleep(1000);
                                }

                                //m_buildData[0].ProgrammingSuccess = true;
                                //m_ecmThreadComplete = true;
                                if (m_buildData[0].ProgrammingSuccess)
                                {
                                    m_ecmResultColor = Color.Green;
                                }
                                else
                                {
                                    m_ecmResultColor = Color.Red;
                                }
                            }
                                m_vehicleCommInterface.ClearResponseBuffer(m_deviceName, m_channelName);
                                if (m_performTCMFlash)
                                {
                                    PreUtilityFileProcessTCM();
                                    ThreadStart taskDelegate1 = null;
                                    taskDelegate1 = new ThreadStart(SequenceTCMInterpreter);
                                    Thread taskThread1 = new Thread(taskDelegate1);
                                    taskThread1.Start();
                                    //while not complete....
                                    while (!m_tcmThreadComplete && GetStatus() == Status.IN_PROGRESS && !m_terminateThreads)
                                    {
                                        Thread.Sleep(1000);
                                    }
                                    if (m_buildData[1].ProgrammingSuccess)
                                    {
                                        m_tcmResultColor = Color.Green;
                                    }
                                    else
                                    {
                                        m_tcmResultColor = Color.Red;
                                    }
                                }
                                //m_tcmThreadComplete = true;
                                //m_buildData[1].ProgrammingSuccess = true;

                        }
                        //stop tester present
                        m_stopTesterPresentThread = true;
                        m_stopProgressBarThread = true;
                        //return ecu to normal mode
                        PostUtilityFileProcess();
                        m_tcmInterpreter = null;
                        m_ecmInterpreter = null;
                    // Repeatability Test}
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT) SetStatus(Status.SUCCESS);
        }
        /*public void TesterPresentThread()
        {

            List<byte> data = new List<byte>();
            while (!m_terminateThreads && !m_stopTesterPresentThread && GetStatus() != Status.ABORT)
            {
                foreach (CcrtJ2534Defs.ECUMessage message in m_testerPresentMessages)
                {                 
                    m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
                }
                System.Threading.Thread.Sleep(TESTER_PRESENT_DELAY);
            }
        }*/
        public void TesterPresentThread()
        {

            List<List<byte>> data = new List<List<byte>>();
            while (!m_terminateThreads && !m_stopTesterPresentThread && GetStatus() != Status.ABORT)
            {
                foreach (CcrtJ2534Defs.ECUMessage message in m_testerPresentMessages)
                {
                    m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);
                }
                /*foreach (ECUBuildData build in m_buildData)
                {//ensure filter always set
                    CcrtJ2534Defs.ECUMessage setFiltermessage = new CcrtJ2534Defs.ECUMessage();
                    List<byte> testPresent = new List<byte>();
                    testPresent.Add(0x3e);
                    setFiltermessage = CreateECUMessage(testPresent, build.RequestID, build.ResponseID);
                    m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                            setFiltermessage.m_messageFilter);
                }*/
                System.Threading.Thread.Sleep(TESTER_PRESENT_DELAY);
            }
        }
        public Status PreUtilityFileProcess()
        {//perform pre utility file all node messages
            Status status = Status.IN_PROGRESS;
            m_logger.Log("INFO:  Performing Pre Utility File Process");
            //start tester present thread
            m_logger.Log("INFO:  Starting Tester Present Thread");
            m_stopTesterPresentThread = false;
            //ensure filter is set up properly
            foreach (ECUBuildData build in m_buildData)
{//ensure filter always set
    CcrtJ2534Defs.ECUMessage setFiltermessage = new CcrtJ2534Defs.ECUMessage();
    List<byte> testPresent = new List<byte>();
    testPresent.Add(0x3e);
    setFiltermessage = CreateECUMessage(testPresent, build.RequestID, build.ResponseID);
    m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            setFiltermessage.m_messageFilter);
}

            //create message to be sent
            List<byte> txMessage = new List<byte>();
            //opcode specific message
            txMessage.Clear();
            txMessage.Add(0xFE);
            txMessage.Add(0x3E);
            m_testerPresentMessages = new List<CcrtJ2534Defs.ECUMessage>();
            /*foreach (ECUBuildData build in m_buildData)
            {
                CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
                message.m_responseExpected = false;
                message.m_rxTimeout = 10;
                m_testerPresentMessages.Add(message);
                m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                        message.m_messageFilter);
            }*/
            // using all nodes message: no response will be received
            
                CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                message = CreateAllNodesMessage(txMessage);
                message.m_responseExpected = false;
                message.m_rxTimeout = 10;
                m_testerPresentMessages.Add(message);
                m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                        message.m_messageFilter);
            
//Skip - Wake up all modules - dont think this is necessary
//Skip - Gateways Wake up all subnets - dont think this is necessary
//Skip - read databyte identifier B0 return ecu diag address - dont think this is necessary
            
            //start tester present thread
            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(TesterPresentThread);
            Thread taskThread = new Thread(taskDelegate);
            taskThread.Start();


            //all nodes disable DTCs
            m_logger.Log("INFO:  Sending Message Disable DTCs");
            txMessage.Clear();
            //txMessage.Add(0xfe);
            txMessage.Add(0x10);
            txMessage.Add(0x02);
            //status = SendMessage(txMessage,true);
            SendMessage(txMessage, false);
            //if (status == Status.SUCCESS)
            if (true)
            {
                //all nodes disable Normal communications
                m_logger.Log("INFO:  Sending Message Disable Normal Communications");
                txMessage.Clear();
                //txMessage.Add(0xfe);
                txMessage.Add(0x28);
                status = SendMessage(txMessage,false);
                //if (status == Status.SUCCESS)
                if (true)
                {
                    //Skip - all nodes report Programmed state

                    //All nodes Request Programming
                    m_logger.Log("INFO:  Sending Message Request Programming");
                    txMessage.Clear();
                    //txMessage.Add(0xfe);
                    txMessage.Add(0xA5);
                    txMessage.Add(0x01);
                    status = SendMessage(txMessage, false);
                    //if (status == Status.SUCCESS)
                    if (true)
                    {
                        //All nodes Enable Programming
                        m_logger.Log("INFO:  Sending Message Enable Programming");
                        txMessage.Clear();
                        //txMessage.Add(0xfe);
                        txMessage.Add(0xA5);
                        txMessage.Add(0x03);
                        status = SendMessage(txMessage, false);
                        status = Status.SUCCESS;
                    }
                }

            }
            if (status != Status.SUCCESS) SetPrompt1("Failure: Pre Utility File Comm Error");
            return status;
        }
        public Status PreUtilityFileProcessTCM()
        {//perform pre utility file all node messages
            Status status = Status.IN_PROGRESS;
            m_logger.Log("INFO:  Performing TCM Pre Utility File Process");

            //Skip - Wake up all modules - dont think this is necessary
            //Skip - Gateways Wake up all subnets - dont think this is necessary
            //Skip - read databyte identifier B0 return ecu diag address - dont think this is necessary
            List<byte> txMessage = new List<byte>();

            //all nodes disable DTCs
            m_logger.Log("INFO:  Sending Message Disable DTCs");
            txMessage.Clear();
            //txMessage.Add(0xfe);
            txMessage.Add(0x10);
            txMessage.Add(0x02);
            //status = SendMessage(txMessage,true);
            SendMessage(txMessage, false);
            //if (status == Status.SUCCESS)
            if (true)
            {
                //all nodes disable Normal communications
                m_logger.Log("INFO:  Sending Message Disable Normal Communications");
                txMessage.Clear();
                //txMessage.Add(0xfe);
                txMessage.Add(0x28);
                status = SendMessage(txMessage, false);
                //if (status == Status.SUCCESS)
                if (true)
                {
                    //Skip - all nodes report Programmed state

                    //All nodes Request Programming
                    m_logger.Log("INFO:  Sending Message Request Programming");
                    txMessage.Clear();
                    //txMessage.Add(0xfe);
                    txMessage.Add(0xA5);
                    txMessage.Add(0x01);
                    status = SendMessage(txMessage, false);
                    //if (status == Status.SUCCESS)
                    if (true)
                    {
                        //All nodes Enable Programming
                        m_logger.Log("INFO:  Sending Message Enable Programming");
                        txMessage.Clear();
                        //txMessage.Add(0xfe);
                        txMessage.Add(0xA5);
                        txMessage.Add(0x03);
                        status = SendMessage(txMessage, false);
                        status = Status.SUCCESS;
                    }
                }

            }
            if (status != Status.SUCCESS) SetPrompt1("Failure: Pre Utility File Comm Error");
            return status;
        }



        public void PostUtilityFileProcess()
        {//perform pre utility file all node messages
            m_logger.Log("INFO:  Performing Post Utility File Process");
            List<byte> txMessage = new List<byte>();
            int retries = 25;
            Status status = Status.ERROR;
            if (m_buildData[0].ProgrammingSuccess || m_buildData[1].ProgrammingSuccess)
            {//write vin hard coded utility file logic flawed
                m_logger.Log("INFO:  Sending Write VIN Message");
                txMessage.Add(0x3B);
                txMessage.Add(0x90);
                txMessage.AddRange(
                m_ecmInterpreter.m_opCodeHandler.stringToASCIIByteArray(m_ecmInterpreter.m_opCodeHandler.GetVIT2Data(0x41)).ToList());
                //retries to allow time for VIN message to respond
                for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                {
                    status = SendMessageConditional(txMessage, true);
                    Thread.Sleep(250);
                }
                if (status == Status.SUCCESS)
                {
                    m_logger.Log("INFO:  Write VIN Message Successful");
                }
                else
                {
                    m_logger.Log("INFO:  Write VIN Message Failure");
                }
                status = Status.ERROR;
            }
            //End Test
          
            txMessage.Clear();


            //all nodes disable DTCs
            m_logger.Log("INFO:  Sending Message Return to normal mode");
            txMessage.Clear();
            txMessage.Add(0x20);
            for (int x = 0; x < retries && status != Status.SUCCESS; x++)
            {
                status = SendMessage(txMessage, true);
                Thread.Sleep(250);
            }
        }
        public Status SendAllNodesMessage(List<byte> txMessage, bool responseExpected)
        {
            List<List<byte>> data = new List<List<byte>>();
            CcrtJ2534Defs.ECUMessage ecuMessage = new CcrtJ2534Defs.ECUMessage();
            ecuMessage = CreateAllNodesMessage(txMessage);
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                ecuMessage.m_messageFilter);
            m_logger.Log("INFO:  Sending Message: " + BitConverter.ToString(txMessage.ToArray()));
            if (m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, ecuMessage, ref data, true))
            {
                m_logger.Log("INFO:  Message Sent");
                if (responseExpected)
                {
                    foreach (List<byte> bytes in data)
                    {
                        m_logger.Log("INFO:  Received: " + BitConverter.ToString(bytes.ToArray()));
                    }
                }
                else
                {
                    m_logger.Log("INFO:  Message sent successfully");
                }
                return Status.SUCCESS;
            }
            else
            {
                m_logger.Log("INFO:  Message Failure");
                return Status.FAILURE;
            }
        }
        public Status SendMessage(List<byte> txMessage, bool responseExpected)
        {
            List<byte> data = new List<byte>();
            bool messageSuccess = true;
            List<CcrtJ2534Defs.ECUMessage> ecuMessages = new List<CcrtJ2534Defs.ECUMessage>();
            foreach (ECUBuildData build in m_buildData)
            {
                CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
                message.m_responseExpected = responseExpected;
                message.m_retries = 1;
                if (txMessage.Count > 4)
                {
                    message.m_txTimeout = 200;
                }
                ecuMessages.Add(message);
                m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                        message.m_messageFilter);
            }

            m_logger.Log("INFO:  Sending Message: " + BitConverter.ToString(txMessage.ToArray()));
            foreach (CcrtJ2534Defs.ECUMessage message in ecuMessages)
            {
                if (m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data))
                {
                    if (responseExpected)
                    {
                        m_logger.Log("INFO:  Received: " + BitConverter.ToString(data.ToArray()));
                    }
                    else
                    {
                        m_logger.Log("INFO:  Message sent successfully");
                    }
                }
                else
                {
                    m_logger.Log("INFO:  Message Failure");
                    messageSuccess = false;
                }

            }
            if (messageSuccess)
            {
                return Status.SUCCESS;
            }
            else
            {
                return Status.FAILURE;
            }
        }
        public Status SendMessageConditional(List<byte> txMessage, bool responseExpected)
        {
            List<byte> data = new List<byte>();
            bool messageSuccess = true;
            List<CcrtJ2534Defs.ECUMessage> ecuMessages = new List<CcrtJ2534Defs.ECUMessage>();
            foreach (ECUBuildData build in m_buildData)
            {
                if (build.ProgrammingSuccess)
                {//only send message if ecu was successfully flashed
                    CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                    message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
                    message.m_responseExpected = responseExpected;
                    message.m_retries = 1;
                    if (txMessage.Count > 4)
                    {
                        message.m_txTimeout = 200;
                    }
                    ecuMessages.Add(message);
                    m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                            message.m_messageFilter);
                }
            }

            m_logger.Log("INFO:  Sending Message: " + BitConverter.ToString(txMessage.ToArray()));
            foreach (CcrtJ2534Defs.ECUMessage message in ecuMessages)
            {
                if (m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data))
                {
                    if (responseExpected)
                    {
                        m_logger.Log("INFO:  Received: " + BitConverter.ToString(data.ToArray()));
                    }
                    else
                    {
                        m_logger.Log("INFO:  Message sent successfully");
                    }
                }
                else
                {
                    m_logger.Log("INFO:  Message Failure");
                    messageSuccess = false;
                }

            }
            if (messageSuccess)
            {
                return Status.SUCCESS;
            }
            else
            {
                return Status.FAILURE;
            }
        }
        //create message using global module response / request id and tx message
        public CcrtJ2534Defs.ECUMessage CreateECUMessage(List<byte> txMessage, List<byte> requestID, List<byte> responseID)
        {
            CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            //add module request ID

            message.m_messageFilter.requestID = requestID.ToArray();
            //add module response ID
            message.m_messageFilter.responseID = responseID.ToArray();

            //Add Tx message
            message.m_txMessage = new List<byte>(txMessage);
            return message;
        }
        //create message using global module response / request id and tx message
        public CcrtJ2534Defs.ECUMessage CreateAllNodesMessage(List<byte> txMessage)
        {
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            //add module request ID
            List<byte> moduleID = new List<byte>();
            moduleID.Add((byte)(0x00));
            moduleID.Add((byte)(0x00));
            moduleID.Add((byte)(0x01));
            moduleID.Add((byte)(0x01));

            message.m_messageFilter.requestID = moduleID.ToArray();
            //add module response IDs
            message.m_messageFilter.responseIDs = new List<byte[]>();
            foreach (ECUBuildData build in m_buildData)
            {
                message.m_messageFilter.responseIDs.Add(build.ResponseID.ToArray());
            }

            //Add Tx message
            message.m_txMessage = new List<byte>(txMessage);
            return message;
        }
        public void OpenECMInterpreter()
        {
            List<string> calFileNames = new List<string>();
            List<string> vit2Data = new List<string>();

            //populate list of cal filenames
            foreach (string pn in m_buildData[0].SoftwareModulePartNumbers)
            {
                if (pn.Substring(0, 1) == "8")
                {
                    calFileNames.Add(m_flashFileDirectory + pn.Substring(FILE_NAME_PN_START_INDEX, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT) + ".pti");
                    vit2Data.Add(pn.Substring(FILE_NAME_PN_START_INDEX, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT));
                }
                else
                {
                    calFileNames.Add(m_flashFileDirectory + pn.Substring(0, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE) + ".pti");
                    //ends with "z0"
                    vit2Data.Add(pn.Substring(0, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE - 2));
                }
            }
            //end order change
            vit2Data.Add(m_buildData[0].VIN);
            m_ecmInterpreter = new Interpreter(m_vehicleCommInterface, ref calFileNames, m_buildData[0].ECUName, vit2Data, m_ecmLogger, m_deviceName, m_channelName);
            m_ecmInterpreter.SetRequestResponseIDPair(0x07E0, 0x07E8);
            //parse utilityFile's Name
            m_ecmInterpreter.m_headerOffset = 0x64;
            m_ecmInterpreter.openUtilityFile(m_flashFileDirectory + m_buildData[0].UtilityFilePartNumber.Substring(FILE_NAME_PN_START_INDEX, UTILITY_FILE_NAME_LENGTH) + ".pti");
            m_ecmInterpreter.m_opCodeHandler.m_header = m_ecmInterpreter.m_header;
            m_ecmDataSizeAdjusted = m_ecmInterpreter.GetDataSize();
            //subtract out data that will not be typically downloaded
            m_ecmDataSizeAdjusted -= m_ecmInterpreter.m_calibrationModules[0].Count();
            m_ecmDataSizeAdjusted -= m_ecmInterpreter.m_calibrationModules[6].Count();
            m_ecmDataSizeAdjusted -= m_ecmInterpreter.m_calibrationModules[7].Count();
            if (m_performECMFlash) m_progressBarRangeMax += m_ecmDataSizeAdjusted;
        }
        public void SequenceECMInterpreter()
        {
            byte gotoByte = 0x01;
            
            m_logger.Log("INFO:  Starting " + m_buildData[0].ECUName + " Flash Process");
            m_buildData[0].ProgrammingSuccess = true;
                //while not terminated and not failure goto sequence interpreter
                while ((GetStatus() == Status.IN_PROGRESS) && (gotoByte != 0xFF) && !m_terminateThreads && m_buildData[0].ProgrammingSuccess)
                {
                    m_logger.Log("INFO:  Processing " + m_buildData[0].ECUName + " Interpreter Instruction: 0x" + Convert.ToString(gotoByte, 16));
                    if (gotoByte <= m_ecmInterpreter.m_interpreterInstructions.Count && gotoByte > 0)
                    {
                        if (gotoByte == 0x02)
                        {//begining of sequence reset bytes transmitted
                            m_ecmInterpreter.m_opCodeHandler.m_bytesTransmitted = 0;
                        }
                        if (m_ecmInterpreter.m_interpreterInstructions[gotoByte - 1].opCode == (byte)OpCodeHandler.GMLANOpCodes.BLOCK_TRANSFER_TO_RAM ||
                            m_ecmInterpreter.m_interpreterInstructions[gotoByte - 1].opCode == (byte)OpCodeHandler.GMLANOpCodes.REQUEST_DOWNLOAD)
                        {
                            if (m_debugMode)
                            {
                                System.Windows.Forms.MessageBox.Show("Waiting For Continue Confirmation", "Debug Mode Enabled",
                                System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Hand);
                            }
                        }
                        gotoByte = m_ecmInterpreter.m_opCodeHandler.ProcessGMLANOpCode(m_ecmInterpreter.m_interpreterInstructions[gotoByte - 1]);
                        if (!m_ecmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_result)
                        {
                            m_resultText = m_ecmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_prompt;
                            SetPrompt1(m_resultText);
                            //SetStatus(Status.FAILURE);
                            m_buildData[0].ProgrammingSuccess = false;
                            m_logger.Log("Failure:  " + m_resultText);
                        }
                        m_buildData[0].ProgrammingSuccess = m_ecmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_result;
                    }
                    else
                    {
                        m_resultText = "Failure: GOTO Byte Specified Out of range";
                        SetPrompt1(m_resultText);
                        //SetStatus(Status.FAILURE);
                        m_logger.Log("Failure:  " + m_resultText);
                    }
                
            }
            m_ecmThreadComplete = true;           
        }
        public void OpenTCMInterpreter()
        {
            List<string> calFileNames = new List<string>();
            List<string> vit2Data = new List<string>();

            //populate list of cal filenames
            foreach (string pn in m_buildData[1].SoftwareModulePartNumbers)
            {
                if (pn.Substring(0, 1) == "8")
                {
                    calFileNames.Add(m_flashFileDirectory + pn.Substring(FILE_NAME_PN_START_INDEX, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT) + ".pti");
                    vit2Data.Add(pn.Substring(FILE_NAME_PN_START_INDEX, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT));
                }
                else
                {
                    calFileNames.Add(m_flashFileDirectory + pn.Substring(0, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE) + ".pti");
                    //ends with "z0"
                    vit2Data.Add(pn.Substring(0, CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE - 2));
                }
            }
            //end order change
            vit2Data.Add(m_buildData[1].VIN);
            m_tcmInterpreter = new Interpreter(m_vehicleCommInterface, ref calFileNames, m_buildData[1].ECUName, vit2Data, m_tcmLogger, m_deviceName, m_channelName);
            m_tcmInterpreter.SetRequestResponseIDPair(0x07E2, 0x07EA);
            //parse utilityFile's Name
            m_tcmInterpreter.m_headerOffset = 0x64;
            m_tcmInterpreter.openUtilityFile(m_flashFileDirectory + m_buildData[1].UtilityFilePartNumber.Substring(FILE_NAME_PN_START_INDEX, UTILITY_FILE_NAME_LENGTH) + ".pti");
            m_tcmInterpreter.m_opCodeHandler.m_header = m_tcmInterpreter.m_header;
            m_tcmDataSizeAdjusted = m_tcmInterpreter.GetDataSize();
            //subtract out data that will not be typically downloaded
            m_tcmDataSizeAdjusted -= m_tcmInterpreter.m_calibrationModules[0].Count();
            if (m_performTCMFlash) m_progressBarRangeMax += m_tcmDataSizeAdjusted;
        }
        public void SequenceTCMInterpreter()
        {
            byte gotoByte = 0x01;
            m_logger.Log("INFO:  Starting " + m_buildData[1].ECUName + " Flash Process");
            m_buildData[1].ProgrammingSuccess = true;
            //while not terminated and not failure goto sequence interpreter
            while ((GetStatus() == Status.IN_PROGRESS) && (gotoByte != 0xFF) && !m_terminateThreads && m_buildData[1].ProgrammingSuccess)
            {
                m_logger.Log("INFO:  Processing " + m_buildData[1].ECUName + " Interpreter Instruction: 0x" + Convert.ToString(gotoByte, 16));
                if (gotoByte <= m_tcmInterpreter.m_interpreterInstructions.Count && gotoByte > 0)
                {
                    if (gotoByte == 0x02)
                    {//begining of sequence reset bytes transmitted
                        m_tcmInterpreter.m_opCodeHandler.m_bytesTransmitted = 0;
                    }
                    if (m_tcmInterpreter.m_interpreterInstructions[gotoByte - 1].opCode == (byte)OpCodeHandler.GMLANOpCodes.BLOCK_TRANSFER_TO_RAM ||
                        m_tcmInterpreter.m_interpreterInstructions[gotoByte - 1].opCode == (byte)OpCodeHandler.GMLANOpCodes.REQUEST_DOWNLOAD)
                    {
                        if (m_debugMode)
                        {
                            System.Windows.Forms.MessageBox.Show("Waiting For Continue Confirmation", "Debug Mode Enabled",
                            System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Hand);
                        }
                    }
                    gotoByte = m_tcmInterpreter.m_opCodeHandler.ProcessGMLANOpCode(m_tcmInterpreter.m_interpreterInstructions[gotoByte - 1]);
                    if (!m_tcmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_result)
                    {
                        m_resultText = m_tcmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_prompt;
                        SetPrompt1(m_resultText);
                        //SetStatus(Status.FAILURE);
                        m_logger.Log("Failure:  " + m_resultText);
                    }
                    m_buildData[1].ProgrammingSuccess = m_tcmInterpreter.m_opCodeHandler.m_currentOpCodeResult.m_result;
                }
                else
                {
                    m_resultText = "Failure: GOTO Byte Specified Out of range";
                    SetPrompt1(m_resultText);
                    //SetStatus(Status.FAILURE);
                    m_buildData[1].ProgrammingSuccess = false;
                    m_logger.Log("Failure:  " + m_resultText);
                }
            }
            m_tcmThreadComplete = true;
        }
        public void ProgressUpdateThread()
        {
            while ((GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && !m_stopProgressBarThread)
            {
                if (!m_ecmThreadComplete && m_performECMFlash)
                {
                    m_progressBarValue = m_ecmInterpreter.m_opCodeHandler.m_bytesTransmitted;
                }
                else if (!m_tcmThreadComplete && m_performTCMFlash)
                {//ecm completed start from estimated size of ecm and add tcm progress
                    if (m_performECMFlash)
                    {
                        m_progressBarValue = m_ecmDataSizeAdjusted + m_tcmInterpreter.m_opCodeHandler.m_bytesTransmitted;
                    }
                    else
                    {
                        m_progressBarValue = m_tcmInterpreter.m_opCodeHandler.m_bytesTransmitted;
                    }
                }
                else
                {//both complete
                    m_progressBarValue = m_progressBarRangeMax;
                }
                Thread.Sleep(500);
            }
        }

        public void ReportDataState()
        {
            ReportData();
            if (GetStatus() != Status.TERMINATE &&
            GetStatus() != Status.ABORT) SetStatus(Status.SUCCESS);
        }
        public void ReportData()
        {
            bool passed = false;
            Prompt prompt = new Prompt();
            if (m_currentBuild != null)
            {//add result and description to file
                XmlNode fsResultNode = m_currentBuild.CreateNode(XmlNodeType.Element,"FlashStationTestResult",null);
                XmlNode resultNode = m_currentBuild.CreateNode(XmlNodeType.Element,"Result",null);
                XmlNode descriptionNode = m_currentBuild.CreateNode(XmlNodeType.Element, "Description", null);
                if (GetStatus() == Status.SUCCESS)
                {
                    SetPrompt1BGColor(Color.Green);
                    m_logger.Log("INFO:  Reporting Pass");
                    resultNode.InnerText = "Pass";
                    passed = true;
                }
                else if (GetStatus() == Status.ABORT || GetPrompt1() == prompt.ABORT_USER1)
                {
                    SetPrompt1BGColor(Color.Yellow);
                    m_logger.Log("INFO:  Reporting Abort");
                    resultNode.InnerText = "Abort";
                    passed = false;
                }
                else
                {// must be in progress check status of module programming
                    bool pass = false;
                    foreach (ECUBuildData bd in m_buildData)
                    {
                        if (bd.PerformFlash)
                        {
                            if (!bd.ProgrammingSuccess)
                            {
                                pass = false;
                                break;
                            }
                            else
                            {
                                pass = true;
                            }
                        }
                        else
                        {
                            pass = true;
                        }
                    }
                    if (!pass)
                    {
                        //programming failed
                        SetPrompt1BGColor(Color.Red);
                        passed = false;
                        m_logger.Log("INFO:  Reporting Fail");
                        resultNode.InnerText = "Fail";
                    }
                    else
                    {
                        //programming passed
                        SetPrompt1(prompt.PASS);
                        SetPrompt1BGColor(Color.Green);
                        passed = true;
                        m_logger.Log("INFO:  Reporting Pass");
                        resultNode.InnerText = "Pass";
                    }
                }
                if (GetStatus() == Status.ABORT)
                {//use prompt text as report details
                    descriptionNode.InnerText = GetPrompt1();
                }
                else
                {//use last interp prompt as detail
                    if (m_resultText == "")
                    {
                        if (passed) m_resultText = "Test Passed";
                        else m_resultText = "Test Failed";
                    }
                    descriptionNode.InnerText = m_resultText;
                }
                fsResultNode.AppendChild(resultNode);
                fsResultNode.AppendChild(descriptionNode);
                m_currentBuild.DocumentElement.AppendChild(fsResultNode);
                int seq = 0;
                DirectoryInfo resultsDirInfo = new DirectoryInfo(m_resultsDirectory);

                foreach(FileInfo fi in resultsDirInfo.GetFiles())
                {
                    if (fi.Name.Length > 17)
                    {
                        if (fi.Name.Substring(0, 17) == m_buildData[0].VIN)
                        {//increment seq number for every file found with this VIN
                            seq++;
                        }
                    }
                }
                //save file
                string filename = m_resultsDirectory + m_buildData[0].VIN + "." + string.Format("{0:000}",seq) + ".xml";
                m_currentBuild.Save(filename);
                //Manage files if we are above the limit
                if (resultsDirInfo.GetFiles().Count() > MAX_RESULT_FILES)
                {//delete oldest
                    FileSystemInfo fileInfo = resultsDirInfo.GetFileSystemInfos()
    .OrderByDescending(fi => fi.CreationTime).First();
                    fileInfo.Delete();
                }
            }

        }

        public void CreateCableConnectCheckMessage()
        {
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x1A);
            txMessage.Add(0x90);
            List<byte> requestID = new List<byte>();
            List<byte> responseID = new List<byte>();
            requestID.Add(0x00);
            requestID.Add(0x00);
            requestID.Add(0x07);
            requestID.Add(0xe0);
            responseID.Add(0x00);
            responseID.Add(0x00);
            responseID.Add(0x07);
            responseID.Add(0xe8);
            m_cableConnectMessage = CreateECUMessage(txMessage, requestID, responseID);
            m_cableConnectMessage.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            m_cableConnectMessage.m_messageFilter);
        }
        public bool IsCableConnected()
        {//drewtech device powered by cable connect - using diagnostic message to determine if cable is connected
            //return m_vehicleCommInterface.IsVehicleConnected(m_deviceName, m_channelName);

            if (m_currentState != StateName.FLASH_ECUS)
            {
                if (!m_usingCableConnectForced)
                {
                    if (m_cableConnectMessage == null)
                    {
                        CreateCableConnectCheckMessage();
                    }
                    List<byte> data = new List<byte>();
                    return m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, m_cableConnectMessage, ref data);
                }
                else
                {
                    return m_cableConnectForced;
                }

            }
            else
                return true;
        }
        public bool ValidBuildDataReceived()
        {
            return m_buildDataValid;
        }
        public List<string> GetAvailableJ2534Devices()
        {
            return m_vehicleCommInterface.GetJ2534DeviceNames();
        }
        public List<string> GetAvailableBarcodeInputDevices()
        {
            return m_inputServerInterface.GetAvailableSerialDeviceNames();
        }
        public List<string> GetAvailableChannels(string deviceName)
        {
            return m_vehicleCommInterface.GetCommChannelNames(deviceName);
        }
        public string GetDeviceName()
        {
            return m_deviceName;
        }
        public string GetBarcodeInputDeviceName()
        {
            return m_barcodeInputDevice;
        }
        public string GetChannelName()
        {
            return m_channelName;
        }
        public void SetDeviceName(string deviceName)
        {//to do add to local database
            m_deviceName = deviceName;
        }
        public void SetBarcodeInputDevice(string deviceName)
        {//to do add to local database
            m_barcodeInputDevice = deviceName;
        }
        public void SetChannelName(string channelName)
        {//to do add to local database
            m_channelName = channelName;
        }
        public void OpenVehicleCommSettings()
        {
            m_vehicleCommInterface.OpenVehicleCommSettingsForm();            
        }
        public void OpenSerialDeviceCommSettings()
        {
            m_inputServerInterface.OpenInputDeviceForm();
        }
        public void Terminate()
        {//stop all active threads            
            m_fcm.StopFCMThread();
            SetStatus(Status.TERMINATE);
            m_terminateThreads = true;
        }
        public void UpdateAvailableBarcodeInputDevices()
        {
            m_inputServerInterface = new InputServerInterface();
        }
        public List<ECUBuildData> GetBuildData()
        {
            return m_buildData;
        }
        public void ToggleForcedCableConnect()
        {
            m_cableConnectForced = !m_cableConnectForced;
        }
        public void ToggleUsingForcedCableConnect()
        {
            m_usingCableConnectForced = !m_usingCableConnectForced;
        }
        public int GetProgressBarValue()
        {
            return m_progressBarValue;
        }
        public int GetProgressBarRangeMin()
        {
            return m_progressBarRangeMin;
        }
        public int GetProgressBarRangeMax()
        {
            return m_progressBarRangeMax;
        }
        /// <summary>
        /// Get/Set the log file directory.
        /// </summary>
        public String LogFileDirectory
        {
            get { return m_logsDirectory; }
            set { m_logsDirectory = value; }
        }
        public Color GetECMResultBGColor()
        {
            return m_ecmResultColor;
        }
        public Color GetTCMResultBGColor()
        {
            return m_tcmResultColor;
        }
        public void SetPerformECMFlash(bool perform)
        {
            m_performECMFlash = perform;
            if (m_buildData.Count == 2)
            {//build data loaded - modify
                m_buildData[0].PerformFlash = perform;
            }
        }
        public void SetPerformTCMFlash(bool perform)
        {
            m_performTCMFlash = perform;
            if (m_buildData.Count == 2)
            {//build data loaded - modify
                m_buildData[1].PerformFlash = perform;
            }
        }

        /// <summary>
        /// Main Form Model Member Variables
        /// </summary>
        private volatile Status m_status;
        private string m_prompt1;
        private string m_prompt2;
        private Color m_prompt1BGColor;
        private Color m_prompt2BGColor;
        private Color m_ecmResultColor;
        private Color m_tcmResultColor;
        private string m_barcode;
        private IVehicleCommServerInterface m_vehicleCommInterface;
        private IInputServerInterface m_inputServerInterface;
        private string m_deviceName;
        private string m_barcodeInputDevice;
        private string m_channelName;
        private bool m_buildDataValid;
        private StateName m_currentState;
        public const int MAX_NUM_SOFTWARE_PART_NOS = 20;
        public const int BARCODE_LENGTH = 30;
        public const int MAX_RESULT_FILES = 1000;
        public const int TESTER_PRESENT_DELAY = 1000;
        private List<ECUBuildData> m_buildData;
        private List<String> m_ecuNames;
        //To do Make Configurable
        private string m_flashFileDirectory = "C:\\FlashStation\\CalFiles\\";
        private string m_buildFileDirectory = "C:\\FlashStation\\BuildFiles\\";
        private string m_tempBuildFileDirectory = "C:\\FlashStation\\TempBuildFiles\\";
        private string m_resultsDirectory = "C:\\FlashStation\\Results\\";
        private string m_logsDirectory = "C:\\FlashStation\\Logs\\";       
        private string m_remoteBuildFileDirectory = "G:\\Rewrite\\Configuration\\VehicleTest\\BuildRecords\\";
        //Test
        //private string m_remoteBuildFileDirectory = "C:\\FlashStation\\BuildRecords\\";
        //End test
        private Logger m_logger;
        public const int FILE_NAME_PN_START_INDEX = 1;
        public const int CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT = 8;
        public const int CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE = 10;
        public const int UTILITY_FILE_NAME_LENGTH = 8;
        private volatile bool m_terminateThreads;
        private volatile bool m_stopTesterPresentThread;
        private volatile bool m_stopProgressBarThread;
        private volatile bool m_cableConnectForced;
        private volatile bool m_usingCableConnectForced;
        private volatile bool m_ecmThreadComplete;
        private volatile bool m_tcmThreadComplete;
        private volatile int m_progressBarValue;
        private volatile int m_progressBarRangeMin;
        private volatile int m_progressBarRangeMax;
        private volatile int m_ecmDataSizeAdjusted;
        private volatile int m_tcmDataSizeAdjusted;
        //change to false for customer deploy
        private volatile bool m_debugMode = false;
        private Logger m_ecmLogger;
        private Logger m_tcmLogger;
        private List<CcrtJ2534Defs.ECUMessage> m_testerPresentMessages;
        private J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage m_cableConnectMessage;
        private XmlDocument m_currentBuild;
        private FCM m_fcm;
        private Interpreter m_ecmInterpreter;
        private Interpreter m_tcmInterpreter;
        private SerialPortLibrary.CcrtSerialPort m_serialPort;
        private bool m_performTCMFlash;
        private bool m_performECMFlash;
    }

}
