using System;
using System.Collections;
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
using FtpFileMonitorNamespace;
using J2534ChannelLibrary;
using System.Drawing;
using System.Windows.Forms;
using Common.MessagesBuilder;
using Common.Models;



namespace Common.Lib.Models
{
    public class MainFormModel  : IMainFormModel
    {
        const string Mimamori_Can = "8975165830";
        const string Mimamori_Kline = "8983499660";
        string mimamoriPartNo;
        /// <summary>
        /// Create a new Presenter responsible for coordinating Model and View
        /// </summary>
        /// <param name="view">View form interface</param>
        /// <param name="logger">logging interface for form display and file output</param>
        public MainFormModel()
        {
           
            m_cableConnectForced = false;
            m_cableConnectMessage = null;
            m_usingCableConnectForced = false;
            //m_keyOffFlag = false;
            //to do get these to program settings
            m_deviceName = "null";
            m_channelName = "null";
            m_mimaChannelName = "KLine";
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
            m_performBASLearn = false;
            m_keyOffEngineOffWaitStart = false;
            m_vehicleCommInterface = new VehicleCommServerInterface();
            m_inputServerInterface = new InputServerInterface();
        }

        public void InitiallizeVehicleComInterface()
        {
            m_vehicleCommInterface = new VehicleCommServerInterface();
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
                        m_flashRetryAttempted = false;
                        SetPrompt1(prompt.WAIT_FOR_USER_INPUT1);
                        SetPrompt2(prompt.WAIT_FOR_USER_INPUT2);
                        SetPrompt1BGColor(Color.White);
                        SetPrompt2BGColor(Color.White);
                        m_tcmResultColor = Color.White;
                        m_ecmResultColor = Color.White;
                        m_dcuResultColor = Color.White;
                        m_mimaResultColor = Color.White;
                        m_brakeApplySensorResultColor = Color.White;
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
                        SetPrompt2(prompt.REPORT_DATA2);
                        taskDelegate = new ThreadStart(ReportDataState);
                        break;
                    case StateName.CHECK_BRAKE_PEDAL:
                        //Check for broken brake switch
                        if (m_performBASLearn)
                        {
                            SetPrompt1(prompt.APPLY_BRAKE);
                        }
                        taskDelegate = new ThreadStart(BrakePedalCheck);
                        break;
                    case StateName.WAIT_FOR_KEY_OFF:
                        if (m_performBASLearn)
                        {
                            SetPrompt1(prompt.KEY_OFF);
                        }
                        taskDelegate = new ThreadStart(WaitForKeyOff);
                        break;
                    case StateName.WAIT_FOR_KEY_ON:
                        if (m_performBASLearn)
                        {
                            SetPrompt1(prompt.KEY_ON);
                            SetPrompt2(prompt.FOOT_OFF_BRAKE);
                        }
                        taskDelegate = new ThreadStart(WaitForKeyOn);
                        break;
                    case StateName.CHECK_FOR_BAS_LEARN:
                        //Check Initial BAS Learn position
                        taskDelegate = new ThreadStart(CheckBASHomePositionLearned);
                        break;
                    case StateName.BAS_LEARN:
                        if (!m_basHomePositionLearned && m_performBASLearn)
                        {
                            SetPrompt1BGColor(Color.Orange);
                            SetPrompt1(prompt.BAS_RELEARN);                           
                        }
                        taskDelegate = new ThreadStart(BASHomePositionRelearn);
                        break;
                    case StateName.WAIT_FOR_CABLEDISCONNECT:
                        //Start Wait for cable connect thread
                        //prompt 1 will be set by report data
                        m_logger.Log("Finished Flash Station Process, disconnect cable");
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
            m_buildDataFcm = new FtpFileMonitor(m_remoteBuildFileLocation, m_buildFileDirectory, m_tempBuildFileDirectory, 10000,
                m_userLogin,m_password,m_ftpServerIp,"*", m_logger);
            m_esnFileChangeMonitor = new FtpFileMonitor(m_remoteESNLocation, m_esnDirectory, m_tempESNDirectory, 10000, 
                m_userLogin, m_password, m_ftpServerIp, "*", m_logger);

            m_passFileFtp = new FtpFileMonitor(m_passIndicationLocation, m_passIndicationLocalDirectory, "", 10000,
                m_userLogin, m_password, m_ftpServerIp,"*", m_logger);

            m_buildDataFcm.StartFileMonitorThread();

            m_esnFileChangeMonitor.StartFileMonitorThread();

            ThreadStart taskDelegate = null;
            taskDelegate = new ThreadStart(PassFileIndicationTransferThread);
            Thread transferPassedTestResults = new Thread(taskDelegate);
            transferPassedTestResults.Start();
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
        public int GetBASTimerCount()
        {
            return m_brakeReleasedCounter;
        }
        public void IncrementBASTimerCount()
        {
            m_brakeReleasedCounter++;
        }
        public void ResetBASTimerCount()
        {
            m_brakeReleasedCounter = 0;
        }
        public bool GetBrakePedalReleased()
        {
            return m_brakeReleased;
        }
        public bool GetBASLearnWaitStart()
        {
            return m_keyOffEngineOffWaitStart;
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
            int attempts = BARCODE_LENGTH;
            while (totalBytesRead < BARCODE_LENGTH && GetBarcode() == "" && GetStatus() == Status.IN_PROGRESS && 
                !m_terminateThreads && (attempts > 0))
            {//wait for user input or scanned data
                bytesRead = m_serialPort.ReadSerialData(interReceivedData, BARCODE_LENGTH);
                if (bytesRead > 0)
                {
                    totalBytesRead += bytesRead;
                    for (int x = 0; x < bytesRead; x++)
                    {
                        receivedData.Add(interReceivedData[x]);
                    }
                    attempts--;
                }
                Thread.Sleep(100);
            }
            if (GetStatus() == Status.IN_PROGRESS)
            {
                barcode = System.Text.Encoding.ASCII.GetString(receivedData.ToArray());
                //ignore input size, this will be checked later
                //if (totalBytesRead == BARCODE_LENGTH)
                //{//set barcode value
                    m_logger.Log("INFO:  Received Barcode Input: " + barcode);
                    SetBarcode(barcode);
                //}
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
                m_modelCode = modelCode;
                string bookCode = barcode.Substring(22, 2);
                string modelYear = barcode.Substring(24, 3);
                string lotNumber = barcode.Substring(27, 3);
                if (lotNumber[0] == 'P')
                {
                    lotNumber = lotNumber.Replace("P", "0");
                }
                string fileName = "Ps" + modelCode + "_" + bookCode + "_" + modelYear + "_";
                buildDataValid = Load(m_buildFileDirectory + fileName, lotNumber, vin);
                m_logger.Log("BuildDataLoaded.");
            }
            else
            {//invalid barcode length
                buildDataValid = false;
                m_logger.Log("ERROR:  Invalid Barcode - Length Required: " + BARCODE_LENGTH.ToString() + " received: " + barcode.Length.ToString());
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
                if (m_buildData[0].ESNWriteRequired)
                {
                    Prompt prompt = new Prompt();
                    m_logger.Log("Getting ESN from "+m_esnDirectory + ". For VIN: " + m_buildData[0].VIN);
                    string tempEsn = "somebullshit";
                    tempEsn = GetESN(m_buildData[0].VIN, m_esnDirectory);
                    tempEsn = tempEsn.Trim(' ');
                    m_logger.Log("Trim ESN");
                    string esn = "";
                    Int32 engineCodeStartIndex = 0;
                    if (tempEsn != "")
                    {   
                        if (tempEsn.Length == m_defaultESNLength)
                        {
                            if (tempEsn[0] == '1')
                            {
                                esn = tempEsn.Remove(5, 1);
                                engineCodeStartIndex = 2;
                            }
                            else
                            {
                                esn = tempEsn;
                                engineCodeStartIndex = 8;
                            }
                            m_buildData[0].EngineSerialNumber = esn;
                            string esnLeadingChars = m_buildData[0].ESNLeadingCharacters;
                            m_logger.Log("Here");
                            if (esnLeadingChars.Length <= m_buildData[0].EngineSerialNumber.Length)
                            {//validate leading characters match
                                if (esnLeadingChars.Length > 0 && m_buildData[0].EngineSerialNumber.Substring(engineCodeStartIndex, esnLeadingChars.Length) != esnLeadingChars)
                                {//fail
                                    buildDataValid = false;
                                    m_logger.Log("ERROR:  ESN Leading Character mismatch ESN: " + m_buildData[0].EngineSerialNumber + "Leading Characters to match: " + esnLeadingChars);
                                    SetPrompt1BGColor(Color.Yellow);
                                    SetPrompt2BGColor(Color.Yellow);
                                    SetPrompt1(prompt.ABORT_INVALID_ESN_VALUE);
                                    SetPrompt2(prompt.ABORT_INVALID_BUILD_DATA2);
                                    SetStatus(Status.ABORT);
                                }
                                else
                                {
                                    SetStatus(Status.SUCCESS);
                                }
                            }
                            
                            else
                            {//failure
                                buildDataValid = false;
                                m_logger.Log("ERROR:  ESN Check length greater than esn");
                                SetPrompt1BGColor(Color.Yellow);
                                SetPrompt2BGColor(Color.Yellow);
                                SetPrompt1(prompt.ABORT_INVALID_ESN_LENGTH);
                                SetPrompt2(prompt.ABORT_INVALID_BUILD_DATA2);
                                SetStatus(Status.ABORT);
                            }
                            m_logger.Log("There");
                        }
                        else
                        {//failure
                            buildDataValid = false;
                            m_logger.Log("ERROR:  ESN Incorrect length: " + tempEsn.Length + " Required: 6");
                            SetPrompt1BGColor(Color.Yellow);
                            SetPrompt2BGColor(Color.Yellow);
                            SetPrompt1(prompt.ABORT_INVALID_ESN_LENGTH);
                            SetPrompt2(prompt.ABORT_INVALID_BUILD_DATA2);
                            SetStatus(Status.ABORT);
                        }
                    }
                    else
                    {
                        buildDataValid = false;
                        m_logger.Log("ERROR:  No ESN found for VIN: " + m_buildData[0].VIN);
                        SetPrompt1BGColor(Color.Yellow);
                        SetPrompt2BGColor(Color.Yellow);
                        SetPrompt1(prompt.ABORT_ESN_NOT_PRESENT);
                        SetPrompt2(prompt.ABORT_INVALID_BUILD_DATA2);
                        SetStatus(Status.ABORT);
                    }
                }
                else
                {// no esn write required
                    m_buildData[0].EngineSerialNumber = "";
                    m_logger.Log("INFO: ESN Write not required");
                    SetStatus(Status.SUCCESS);
                }
            }
            m_logger.Log("GotESN");
            m_buildDataValid = buildDataValid;
        }
        /// <summary>
        /// Load build file
        /// </summary>
        public bool Load(string fileName, string lotNo,string vin)
        {
            m_logger.Log("FN: Entering Load");

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
                    lock (m_buildDataFcm.DirectoryLock)
                    {
                        m_currentBuild = new XmlDocument();
                        m_currentBuild.Load(fileName + lotNo + ".xml");
                    }
                    //for each ecu...
                    foreach (string ecuName in m_ecuNames)
                    {
                        m_logger.Log("ECM: "+ecuName);
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
                        //get partnum
                        if (ecuName == "TCM")
                        {
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x07);
                            ecuBuild.RequestID.Add(0xe2);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x07);
                            ecuBuild.ResponseID.Add(0xeA);
                            if (m_isFlashRequired)
                            {
                                searchNode = buildFileECUName + "ASM" + "PartNo";
                                ecuBuild.PerformFlash = m_performTCMFlash;
                            }
                            else
                                ecuBuild.PerformFlash = false;
                        }
                        else if (ecuName == "DCU")
                        {
                            searchNode = buildFileECUName + "PartNo";
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x00);
                            ecuBuild.RequestID.Add(0x02);
                            ecuBuild.RequestID.Add(0x4B);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x00);
                            ecuBuild.ResponseID.Add(0x06);
                            ecuBuild.ResponseID.Add(0x4B);
                            ecuBuild.PerformFlash = m_performDCUFlash;
                            m_logger.Log("DCU Done:");
                        }
                        else if (ecuName == "Mimamori")
                        {
                            m_logger.Log("Mimamori Loading Configs:");
                            searchNode = buildFileECUName + "PartNo";
                            m_logger.Log("SearchNode: "+searchNode);
                            ecuBuild.PerformFlash = m_performMimamoriFlash;
                            mimamoriPartNo = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Found: " + mimamoriPartNo);
                            
                            searchNode = "Transmission";
                            m_logger.Log("SearchNode: " + searchNode);
                            ecuBuild.Transmission = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Found: " + m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText);
                            
                            searchNode = "Rearaxelratio";
                            m_logger.Log("SearchNode: " + searchNode);
                            ecuBuild.RearAxel = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Found: " + m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText);

                            searchNode = "Tiresize";
                            m_logger.Log("SearchNode: " + searchNode);
                            ecuBuild.TireSize = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Found: " + m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText);
                            searchNode = "Speedmeter";
                            m_logger.Log("SearchNode: " + searchNode);
                            ecuBuild.SpeedMeter = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Found: " + m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText);
                        }
                        else
                        {
                            m_logger.Log("ECM Else:");
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
                        //get utility file pn
                        if (ecuName != "TCM" || m_isFlashRequired)
                        {
                            if (ecuName != "TCM")
                            {
                                m_logger.Log("Searching for PartNo:");
                                searchNode = buildFileECUName + "PartNo";
                            }
                            else
                            {
                                m_logger.Log("Searching for ASMPartNo:");
                                searchNode = buildFileECUName + "ASM" + "PartNo";
                            }
                            ecuBuild.PartNumber = m_currentBuild.GetElementsByTagName(searchNode)[0].InnerText;
                            m_logger.Log("Got PartNo:");
                            searchNode = buildFileECUName + "UtilityFilePartNo";
                            XmlNodeList list = m_currentBuild.GetElementsByTagName(searchNode);
                            m_logger.Log("Got UtilPartNo:");
                            ecuBuild.UtilityFilePartNumber = list[0].InnerText;  
                        }
                        
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
                        try
                        {
                            ecuBuild.ESNWriteRequired = (m_currentBuild.GetElementsByTagName("WriteESN")[0].InnerText == "True");
                            ecuBuild.ESNLeadingCharacters = m_currentBuild.GetElementsByTagName("ESNLeadingCharacters")[0].InnerText;
                        }
                        catch
                        {
                            ecuBuild.ESNWriteRequired = false;
                            ecuBuild.ESNLeadingCharacters = "";
                        }
                        m_logger.Log("Adding " + ecuName);
                        m_buildData.Add(ecuBuild);
                        m_logger.Log("ECM: " + ecuName +"Added!");
                    }
                }
                else
                {//error
                    success = false;
                    m_logger.Log("ERROR:  No matching file found for file prefix: " + fileName);

                }
            }
            catch(Exception e)
            {//error
                success = false;
                m_logger.Log("ERROR:  Exception loading file: " + fileName + lotNo + ".xml - ");
                m_logger.Log("" + e.ToString());
                m_logger.Log("Filename: " + fileName + "  LotNo: " + lotNo + "  VIN:" + vin);
                
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

        public void WaitForKeyOn()
        {
            int timeoutCount = 0;
            if (m_isFlashRequired && m_performBASLearn)
            {
                while (!IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)/*165*/
                {
                    Thread.Sleep(500);
                    timeoutCount++;
                }
                //Add timeout check at 1 min in case the CARDAQ disconnects
                if (timeoutCount == 85)
                {
                    if (!ReconnectCurrentDevice())
                    {
                        Prompt prompt = new Prompt();
                        SetStatus(Status.TIMEOUT);
                        SetPrompt1BGColor(Color.DeepPink);
                        SetPrompt1(prompt.TIMEOUT);
                        SetPrompt2BGColor(Color.DeepPink);
                        SetPrompt2(prompt.TIMEOUT2);
                    }
                }
            }
            else
            {
                m_logger.Log("Skipping BAS Learn Key On Check");
            }
            if (GetStatus() != Status.TERMINATE && GetStatus() != Status.ABORT && GetStatus() != Status.TIMEOUT)
                SetStatus(Status.SUCCESS);
        }
        public void WaitForKeyOff()
        {
            int timeoutCount = 0; 
            if (m_isFlashRequired && m_performBASLearn)
            {
                while (IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85) /*165*/
                {
                    Thread.Sleep(500);
                    timeoutCount++;
                }
                //Add timeout check at 1 min in case the CARDAQ disconnects
                if (timeoutCount == 85)
                {
                    
                    if (!ReconnectCurrentDevice())
                    {
                        Prompt prompt = new Prompt();
                        SetStatus(Status.TIMEOUT);
                        SetPrompt1BGColor(Color.DeepPink);
                        SetPrompt1(prompt.TIMEOUT);
                        SetPrompt2BGColor(Color.DeepPink);
                        SetPrompt2(prompt.TIMEOUT2);
                        return;
                    }
                }
            }
            else
            {
                m_logger.Log("Skipping BAS Learn Key Off Check");
            }
            if (GetStatus() != Status.TERMINATE && GetStatus() != Status.ABORT)
                SetStatus(Status.SUCCESS); 
        }
        public void BrakePedalCheck()
        {
            int timeoutCount = 0;
            bool brakeSwitchOn = false;
            bool brakeSwitchOff = false;
            bool brakeSwitchFailure = false;
            if (m_isFlashRequired && m_performBASLearn)
            {
                Prompt prompt = new Prompt();
                while (!brakeSwitchOn && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 135) /*255*/
                {
                    Thread.Sleep(250);
                    timeoutCount++;
                    brakeSwitchOn = IsBrakePedalApplied();
                }
                //Add timeout check at 1 min in case the CARDAQ disconnects
                if(timeoutCount == 135)
                {
                    if (!ReconnectCurrentDevice())
                    {
                        SetStatus(Status.TIMEOUT);
                        SetPrompt1BGColor(Color.DeepPink);
                        SetPrompt1(prompt.TIMEOUT);
                        SetPrompt2BGColor(Color.DeepPink);
                        SetPrompt2(prompt.TIMEOUT2);
                    }
                }
                else
                {
                    timeoutCount = 0;
                    SetPrompt1(prompt.RELEASE_BRAKE);
                    while (!brakeSwitchOff && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 135) /*255*/
                    {
                        Thread.Sleep(250);
                        timeoutCount++;
                        brakeSwitchOff = !IsBrakePedalApplied();
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 135)
                    {
                        if (!ReconnectCurrentDevice()) 
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                        }
                    }
                }
                if (!brakeSwitchOff || !brakeSwitchOn)
                {
                    SetStatus(Status.FAILURE);
                    m_brakeApplySensorResultColor = Color.Red;
                    SetPrompt1(prompt.PEDAL_FAILURE);
                    SetPrompt1BGColor(Color.Red);
                    brakeSwitchFailure = true;
                }
            }
            else
            {
                m_logger.Log("Skipping BAS Learn Brake Pedal Check");
            }

            if (GetStatus() != Status.TERMINATE && GetStatus() != Status.ABORT && GetStatus() != Status.TIMEOUT && !brakeSwitchFailure)
                SetStatus(Status.SUCCESS);        
        }
        public void WaitForBrakeApplied()
        {
            while (!IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
            {
                Thread.Sleep(500);
            }
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT)
                SetStatus(Status.SUCCESS);
        }
        public void WaitForBrakeReleased()
        {
            while (IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
            {
                Thread.Sleep(500);
            }
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT)
                SetStatus(Status.SUCCESS);
        }
        public void CreateKeyOnMessage()
        {
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x22);
            txMessage.Add(0x20);
            txMessage.Add(0xD0);
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
            m_keyOnMessage = CreateECUMessage(txMessage, requestID, responseID);
            m_keyOnMessage.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            m_keyOnMessage.m_messageFilter);
        }
        public CcrtJ2534Defs.ECUMessage CreateBASLearnMessage()
        {
            CcrtJ2534Defs.ECUMessage message;
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x22);
            txMessage.Add(0x11);
            txMessage.Add(0x24);
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
            message = CreateECUMessage(txMessage, requestID, responseID);
            message.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            message.m_messageFilter);
            return message;
        }
        public CcrtJ2534Defs.ECUMessage CreateBASPositionMessage()
        {   
            CcrtJ2534Defs.ECUMessage message;
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x22);
            txMessage.Add(0x20);
            txMessage.Add(0xD2);
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
            message = CreateECUMessage(txMessage, requestID, responseID);
            message.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            message.m_messageFilter);
            return message;
        }
        public CcrtJ2534Defs.ECUMessage CreateBASRelearnMessage()
        {
            CcrtJ2534Defs.ECUMessage message;
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0xAE);
            txMessage.Add(0x20);
            txMessage.Add(0x00);
            txMessage.Add(0x00);
            txMessage.Add(0x00);
            txMessage.Add(0x00);
            txMessage.Add(0x02);
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
            message = CreateECUMessage(txMessage, requestID, responseID);
            message.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            message.m_messageFilter);
            return message;
        }
        public CcrtJ2534Defs.ECUMessage CreateBrakePedalPositionMessage()
        {
            CcrtJ2534Defs.ECUMessage message;
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x22);
            txMessage.Add(0x11);
            txMessage.Add(0x24);
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
            message = CreateECUMessage(txMessage, requestID, responseID);
            message.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
            message.m_messageFilter);
            return message;
        }
        public bool IsKeyOn()
        {
            if (m_keyOnMessage == null)
            {
                CreateKeyOnMessage();
            }
            List<byte> data = new List<byte>();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, m_keyOnMessage, ref data);
            m_logger.Log("INFO:  IsKeyOn() Received: " + BitConverter.ToString(data.ToArray()));

            //Check Engine on
            if (data.Count > 3 && (data[3] & 0xC0) == 0xC0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        public bool IsBASHomePositionLearned()
        {
            CcrtJ2534Defs.ECUMessage message = CreateBASLearnMessage();
            List<byte> data = new List<byte>();
            bool moduleReportsLearnComplete = false;
            bool modulePositionOK = false;
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
            m_logger.Log("INFO:  IsBASHomePositionLearned() Received: " + BitConverter.ToString(data.ToArray()));
            if (data.Count > 3)
            {
                if ((data[3] & 0x01) == 0x01)
                {
                    moduleReportsLearnComplete = true;
                }
                else
                {
                    moduleReportsLearnComplete = false;
                }
            }
            else
            {
                m_logger.Log("No Data Received for BAS Home position learned");
                return false;
            }

            m_logger.Log("INFO: Checking BAS Position");
            message = CreateBASPositionMessage();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
            m_logger.Log("INFO:  GetBASHomePosition() Received: " + BitConverter.ToString(data.ToArray()));
            if (data.Count > 0)
            {
                if ((data[3] & 0xFF) >= 0x33 && (data[3] & 0xFF) <= 0x4D)
                {
                    modulePositionOK = true;
                }
                else
                {
                    modulePositionOK = false;
                }
            }
            else
            {
                m_logger.Log("No Data Received for BAS Home position value");
                return false;
            }
            
            return moduleReportsLearnComplete && modulePositionOK;

        }
        public bool IsBrakePedalApplied()
        {
            CcrtJ2534Defs.ECUMessage message = CreateBrakePedalPositionMessage();
            List<byte> data = new List<byte>();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
            m_logger.Log("INFO:  IsBrakePedalApplied() Received: " + BitConverter.ToString(data.ToArray()));
            if (data.Count > 3)
            {
                if ((data[3] & 0x08) == 0x00)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else 
                return false;
        }
        public bool IsBASLearnRequestSuccessful()
        {
            CcrtJ2534Defs.ECUMessage message = CreateBASRelearnMessage();
            List<byte> data = new List<byte>();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
            m_logger.Log("INFO:  IsBASLearnRequestSucessful() Received: " + BitConverter.ToString(data.ToArray()));
            if (data.Count > 0)
            {
                if ((data[0]) == 0x7F)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
                return false;
        }
        public void CheckBASHomePositionLearned()
        {
            if (m_isFlashRequired && m_performBASLearn && !IsBrakePedalApplied())
            {
                //delay 800 ms for BAS Learn
                Thread.Sleep(800);
                m_basHomePositionLearned = IsBASHomePositionLearned();
            }
            else
            {
                m_logger.Log("Skipping BAS Learn Check");
                if(IsBrakePedalApplied())
                    m_basHomePositionLearned = false;
            }
            SetStatus(Status.SUCCESS);
        }
        public void BASHomePositionRelearn()
        {
            Prompt prompt = new Prompt();
            Int32 brakeReleasedCounter = 0;
            int timeoutCount = 0;
            if (m_isFlashRequired && !m_basHomePositionLearned && m_performBASLearn)
            {
                m_logger.Log("Sending CPID Request for BAS relearn\n");
                //Send CPID
                if (IsBASLearnRequestSuccessful() || true)/*REMOVE BEFORE INSTALL*/
                {
                    m_logger.Log("BAS relearn request successful, starting relearn process\n");
                    //IGN off for 2 minutes
                    SetPrompt2(prompt.KEY_OFF);
                    while (IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice()) 
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    SetPrompt2(prompt.FOOT_OFF_BRAKE);
                    m_logger.Log("Ignition off, starting 2 minute wait\n");
                    while (brakeReleasedCounter <= 325 && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
                    {
                        m_brakeReleased = !IsBrakePedalApplied();
                        Thread.Sleep(50);
                        brakeReleasedCounter++;
                    }
                    m_logger.Log("Finished 2 minute wait\n");
                    timeoutCount = 0;
                    //IGN on
                    SetPrompt2(prompt.KEY_ON);
                    while (!IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85) /*165*/
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    timeoutCount = 0;
                    m_logger.Log("Ignition on, starting brake pedal sequence\n");
                    //Depress brake
                    SetPrompt2(prompt.APPLY_BRAKE);
                    while (!IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)/*165*/
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    SetPrompt2(prompt.RELEASE_BRAKE);
                    timeoutCount = 0;
                    while (IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    SetPrompt2(prompt.FOOT_OFF_BRAKE);
                    //Wait 10 seconds
                    m_logger.Log("Starting 10 second wait\n");
                    Thread.Sleep(10000); //Change the prompts for this so the driver will know what is happening
                    m_logger.Log("End 10 second wait\n");
                    //Depress brake
                    SetPrompt2(prompt.APPLY_BRAKE);
                    timeoutCount = 0;
                    while (!IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85) /*165*/
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    timeoutCount = 0;
                    SetPrompt2(prompt.RELEASE_BRAKE);
                    while (IsBrakePedalApplied() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    m_logger.Log("End brake pedal sequence\n");
                    timeoutCount = 0;
                    //IGN off for 2 minutes
                    SetPrompt2(prompt.KEY_OFF);
                    while (IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85)
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 1 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    SetPrompt2(prompt.FOOT_OFF_BRAKE);
                    m_keyOffEngineOffWaitStart = true;
                    timeoutCount = 0;
                    brakeReleasedCounter = 0;
                    m_logger.Log("Ignition off, starting 2 minute wait\n");
                    while (brakeReleasedCounter <= 325 && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads)
                    {
                        m_brakeReleased = !IsBrakePedalApplied();
                        Thread.Sleep(50);
                        brakeReleasedCounter++;
                    }
                    m_keyOffEngineOffWaitStart = false;
                    m_logger.Log("Finished 2 minute wait\n");
                    timeoutCount = 0;
                    SetPrompt2(prompt.KEY_ON);
                    while (!IsKeyOn() && (GetStatus() == Status.IN_PROGRESS) && !m_terminateThreads && timeoutCount < 85) /*165*/
                    {
                        Thread.Sleep(500);
                        timeoutCount++;
                    }
                    //Add timeout check at 2 min in case the CARDAQ disconnects
                    if (timeoutCount == 85)
                    {
                        if (!ReconnectCurrentDevice())
                        {
                            SetStatus(Status.TIMEOUT);
                            SetPrompt1BGColor(Color.DeepPink);
                            SetPrompt1(prompt.TIMEOUT);
                            SetPrompt2BGColor(Color.DeepPink);
                            SetPrompt2(prompt.TIMEOUT2);
                            return;
                        }
                    }
                    m_logger.Log("Ignition on, checking BAS Learn Status\n");
                    if (IsBASHomePositionLearned())
                    {
                        SetStatus(Status.SUCCESS);
                        m_brakeRelearnPassed = true;
                        m_brakeApplySensorResultColor = Color.Green;
                        SetPrompt1(prompt.BAS_PASS);
                    }
                    else
                    {
                        m_logger.Log("BAS Relearn not successful\n");
                        SetStatus(Status.FAILURE);
                        m_brakeRelearnPassed = false;
                        m_brakeApplySensorResultColor = Color.Red;
                        SetPrompt1(prompt.BAS_FAILURE);
                    }
                }
                else
                {
                    m_logger.Log("CPID request for BAS Relearn not successful\n");
                    SetStatus(Status.FAILURE);
                    m_brakeRelearnPassed = false;
                    m_brakeApplySensorResultColor = Color.Red;
                    SetPrompt1(prompt.BAS_FAILURE);
                }
                SetPrompt1BGColor(m_brakeApplySensorResultColor);
                m_logger.Log("Exit BAS Relearn Function\n");
            }
            else
            {
                if (m_basHomePositionLearned)
                {
                    m_logger.Log("Skipping BAS ReLearn because Inital Learn Passed");
                    m_brakeApplySensorResultColor = Color.Green;
                    SetPrompt1(prompt.BAS_PASS);
                    m_brakeRelearnPassed = true;
                }
                else
                {
                    m_logger.Log("Skipping BAS ReLearn");
                    m_brakeRelearnPassed = false;
                }
                SetStatus(Status.SUCCESS);
            }
        }
       
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
            if (m_isFlashRequired)
            {
                for (int x = 0; x < FLASH_RETRIES; x++)
                {
                    m_progressBarValue = 0;
                    m_logger.Log("INFO:  Starting Flash Process Attempt: " + x.ToString());
                    if (x < 0)
                    {
                        m_flashRetryAttempted = true;
                    }
                    Status preFileProcessStatus = Status.FAILURE;
                    m_vehicleCommInterface.ClearResponseBuffer(m_deviceName, m_channelName);
                    preFileProcessStatus = PreUtilityFileProcess();
                    OpenTCMInterpreter();
                    OpenECMInterpreter();
                    m_resultText = "";
                    m_ecmThreadComplete = false;
                    m_tcmThreadComplete = false;
                    m_dcuThreadComplete = false;
                    m_mimaThreadComplete = false;
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
                        m_vehicleCommInterface.ClearResponseBuffer(m_deviceName, m_channelName);
                    }
                    //stop tester present
                    m_stopTesterPresentThread = true;
                    m_stopProgressBarThread = true;
                    //return ecu to normal mode
                    PostUtilityFileProcess();
                    m_logger.Log("Finished PostUtilityFileProcess");
                    m_tcmInterpreter = null;
                    m_ecmInterpreter = null;
                    m_dcuInterpreter = null;
                    m_mimaInterpreter = null;
                    if (m_buildData[0].ProgrammingSuccess && m_buildData[1].ProgrammingSuccess)
                    {//break out of retry for loop
                        break;
                    }
                }
            }
            else
            {
                ProgramVINAndESN();
                CalibrateMimamori();
                m_stopTesterPresentThread = true;
            }
            // Repeatability Test}.
            if (GetStatus() != Status.TERMINATE &&
                GetStatus() != Status.ABORT) SetStatus(Status.SUCCESS);
        }
        public void TesterPresentThread()
        {

            List<List<byte>> data = new List<List<byte>>();
            while (!m_terminateThreads && !m_stopTesterPresentThread && GetStatus() != Status.ABORT)
            {
                foreach (CcrtJ2534Defs.ECUMessage message in m_testerPresentMessages)
                {
                    m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);
                }
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
            txMessage.Add(0x10);
            txMessage.Add(0x02);    
            SendMessage(txMessage, false);

            txMessage.Clear();
            txMessage.Add(0xFE);
            txMessage.Add(0x10);
            txMessage.Add(0x02);
            message = new CcrtJ2534Defs.ECUMessage();
            message = CreateAllNodesMessage(txMessage);
            message.m_responseExpected = false;
            message.m_rxTimeout = 10;
            List<List<byte>> data = new List<List<byte>>();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);
            //if (status == Status.SUCCESS)
            if (true)
            {
                //all nodes disable Normal communications
                m_logger.Log("INFO:  Sending Message Disable Normal Communications");
                txMessage.Clear();
                txMessage.Add(0x28);
                status = SendMessage(txMessage,false);
                txMessage.Clear();
                txMessage.Add(0xFE);
                txMessage.Add(0x28);
                message = new CcrtJ2534Defs.ECUMessage();
                message = CreateAllNodesMessage(txMessage);
                message.m_responseExpected = false;
                message.m_rxTimeout = 10;
                data = new List<List<byte>>();
                m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);

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
            txMessage.Add(0x10);
            txMessage.Add(0x02);    
            SendMessage(txMessage, false);
            m_logger.Log("INFO:  Sending Message Read MEC Counter");
            txMessage.Clear();
            txMessage.Add(0x1A);
            txMessage.Add(0xA0);
            SendMessage(txMessage, false);
            txMessage.Clear();
            txMessage.Add(0xFE);
            txMessage.Add(0x10);
            txMessage.Add(0x02);
            CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            message = CreateAllNodesMessage(txMessage);
            message.m_responseExpected = false;
            message.m_rxTimeout = 10;
            List<List<byte>> data = new List<List<byte>>();
            m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);
            //if (status == Status.SUCCESS)
            if (true)
            {
                //all nodes disable Normal communications
                m_logger.Log("INFO:  Sending Message Disable Normal Communications");
                txMessage.Clear();
                txMessage.Add(0x28);
                status = SendMessage(txMessage,false);
                txMessage.Clear();
                txMessage.Add(0xFE);
                txMessage.Add(0x28);
                message = new CcrtJ2534Defs.ECUMessage();
                message = CreateAllNodesMessage(txMessage);
                message.m_responseExpected = false;
                message.m_rxTimeout = 10;
                data = new List<List<byte>>();
                m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data, true);

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
            if (m_performECMFlash || m_performTCMFlash)
            {
                //Write VIN
                txMessage.Add(0x3B);
                txMessage.Add(0x90);
                txMessage.AddRange(
                m_ecmInterpreter.m_opCodeHandler.stringToASCIIByteArray(m_ecmInterpreter.m_opCodeHandler.GetVIT2Data(0x41)).ToList());
                foreach (ECUBuildData build in m_buildData)
                {
                    if (build.ProgrammingSuccess)
                    {//only send message if ecu was successfully flashed
                        status = Status.ERROR;
                        m_logger.Log("INFO:  Sending Write VIN Message");
                        //retries to allow time for VIN message to respond
                        for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                        {
                            status = SendMessage(build, txMessage, true);
                            Thread.Sleep(250);
                        }
                        if (status == Status.SUCCESS)
                        {
                            m_logger.Log("INFO:  Write VIN Message Successful");
                        }
                        else
                        {
                            m_logger.Log("INFO:  Write VIN Message Failure");
                            build.VinWriteSuccess = false;
                        }
                    }
                }
                if (m_buildData[0].ESNWriteRequired)
                {
                    if (m_buildData[0].ProgrammingSuccess && m_performECMFlash)
                    {// ECM flash successful and model requires ESN write
                        string esn = m_buildData[0].EngineSerialNumber;
                        //write ESN
                        txMessage.Clear();
                        status = Status.ERROR;
                        m_logger.Log("INFO:  Sending Write ESN Message");
                        txMessage.Add(0x3B);
                        txMessage.Add(0x17);
                        //ESN length message must be 17 characters with fill bytes preceded being 0x00
                        if (esn.Length < 17)
                        {
                            for (int x = esn.Length; x < 17; x++)
                                txMessage.Add(0x00);
                        }
                        txMessage.AddRange(
                        m_ecmInterpreter.m_opCodeHandler.stringToASCIIByteArray(esn).ToList());
                        //retries to allow time for ESN message to respond
                        for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                        {
                            status = SendMessage(m_buildData[0], txMessage, true);
                            Thread.Sleep(250);
                        }
                        if (status == Status.SUCCESS)
                        {
                            m_logger.Log("INFO:  Write ESN Message Successful");
                            //read ESN
                            txMessage.Clear();
                            m_logger.Log("INFO:  Sending read ESN Message");
                            txMessage.Add(0x1A);
                            txMessage.Add(0x17);
                            SendMessage(m_buildData[0], txMessage, true);
                        }
                        else
                        {
                            m_logger.Log("INFO:  Write ESN Message Failure");
                            m_buildData[0].SerialNumberWriteSuccess = false;
                        }
                    }
                    else
                    {
                        m_logger.Log("INFO:  ECM Not Flashed Do not write ESN");
                    }
                }
                else
                {
                    m_logger.Log("INFO:  No ESN Write Required");
                }

                m_logger.Log("INFO:  Sending Message Return to normal mode");
                txMessage.Clear();
                txMessage.Add(0x20);
                status = Status.ERROR;
                /*for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                {*/
                    status = SendMessage(txMessage, true);
                    Thread.Sleep(250);
                //}

                txMessage.Clear();

                //all nodes disable DTCs
                m_logger.Log("INFO:  Sending Message Return to normal mode");
                txMessage.Clear();
                txMessage.Add(0x20);
                status = Status.ERROR;
                /*for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                {*/
                    status = SendMessage(txMessage, true);
                    Thread.Sleep(250);
                //}
                //all nodes clear faults
                m_logger.Log("INFO:  Sending Message Clear faults");
                txMessage.Clear();
                txMessage.Add(0xFD);
                txMessage.Add(0x04);
                CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                message = CreateAllNodesMessage(txMessage);
                message.m_responseExpected = false;
                message.m_rxTimeout = 100;
                List<List<byte>> datalist = new List<List<byte>>();
                m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref datalist, true);
                m_logger.Log("Finished Clear Faults Message");
                Thread.Sleep(100);
            }
            m_logger.Log("exiting Post Utility File Process");
            Thread.Sleep(100);
        }
        public void ProgramVINAndESN()
        {
            m_logger.Log("INFO:  Programming VIN and ESN");
            List<byte> txMessage = new List<byte>();
            Status status = Status.ERROR;
            List<byte> testData = new List<byte>();
            CcrtJ2534Defs.ECUMessage testMessage;
            byte[] temp;
            bool flashECMSuccess = true, flashDCUSuccess = true;

            //Enter Diagnostic Mode
            txMessage.Clear();
            txMessage.Add(0x10);
            txMessage.Add(0x02);

            testMessage = CreateECUMessage(txMessage, m_buildData[0].RequestID, m_buildData[0].ResponseID);
            testMessage.m_txTimeout = 200;
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName, testMessage.m_messageFilter);
            Thread.Sleep(250);


            foreach (ECUBuildData build in m_buildData)
            {
                if(build.ECUName == "ECM" && m_performECMFlash || build.ECUName == "DCU" && m_performDCUFlash)
                {

                    status = Status.ERROR;
                    m_logger.Log("INFO:  Sending Message Write Vin");
                    //Write VIN
                    testData.Clear();
                    txMessage.Clear();
                    txMessage.Add(0x3B);
                    txMessage.Add(0x90);
                    temp = Encoding.ASCII.GetBytes(m_buildData[0].VIN);
                    txMessage.AddRange(temp);
                    //retries to allow time for VIN message to respond
                    status = SendMessage(build, txMessage, true);
                    Thread.Sleep(250);

                    if (status == Status.SUCCESS)
                    {
                        m_logger.Log("INFO:  Write VIN Message Successful");
                    }
                    else
                    {
                        m_logger.Log("INFO:  Write VIN Message Failure");
                        build.VinWriteSuccess = false;
                        if (build.ECUName == "DCU")
                            flashDCUSuccess = false;
                        else
                            flashECMSuccess = false;
                    }
                    //Check Calibration Part Number
                    status = Status.ERROR;
                    m_logger.Log("INFO:  Sending Read Part Number Message");
                    //retries to allow time for message to respond
                    txMessage.Clear();
                    txMessage.Add(0x1A);
                    txMessage.Add(0xCB);
                    List<byte> data = new List<byte>();
                    CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                    message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
                    message.m_responseExpected = true;
                    bool messageSuccess = m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
                    Thread.Sleep(250);
                    char[] tempData = new char[8];
                    int j = 0;
                    //hardcoded length to 6
                    for (int i = 2; i < 6; i++ )
                    {
                        tempData[j] = ((char)(data[i] / 16 + 48));
                        tempData[j + 1] = ((char)(data[i] % 16 + 48));
                        j+=2;
                    }
                    string partNumber = new string(tempData);
                    status = messageSuccess ? Status.SUCCESS : Status.FAILURE;
                    if (status == Status.SUCCESS)
                    {
                        m_logger.Log("INFO:  Read Part Number Successful");
                        //Check if part number matches the part number in the build record
                        //if (build.PartNumber.CompareTo(partNumber) == 0)
                        if(build.UtilityFilePartNumber.Substring(1,8).CompareTo(partNumber) == 0)
                        {
                            m_logger.Log("INFO:  Calibration Part Numbers Match");
                            build.PartNumberMatches = true;
                        }
                        else
                        {
                            m_logger.Log("INFO:  Calibration Part Numbes don't match");
                            build.PartNumberMatches = false;
                            if (build.ECUName == "DCU")
                                flashDCUSuccess = false;
                            else
                                flashECMSuccess = false;
                        }
                    }
                    else
                    {
                        m_logger.Log("INFO:  Read Calibration Part Number Failure");
                        if (build.ECUName == "DCU")
                            flashDCUSuccess = false;
                        else
                            flashECMSuccess = false;
                    }
                    /*Check Part Number
                    status = Status.ERROR;
                    Thread.Sleep(50);
                    m_logger.Log("INFO:  Sending Read Part Number Message");
                    //retries to allow time for message to respond
                    txMessage.Clear();
                    txMessage.Add(0x1A);
                    txMessage.Add(0xCC);
                    data = new List<byte>();
                    message = new CcrtJ2534Defs.ECUMessage();
                    message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
                    message.m_responseExpected = true;
                    messageSuccess = m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
                    Thread.Sleep(250);
                    tempData = new char[8];
                    j = 0;
                    //hardcoded length to 6
                    for (int i = 2; i < 6; i++)
                    {
                        tempData[j] = ((char)(data[i] / 16 + 48));
                        tempData[j + 1] = ((char)(data[i] % 16 + 48));
                        j += 2;
                    }
                    partNumber = new string(tempData);
                    status = messageSuccess ? Status.SUCCESS : Status.FAILURE;
                    if (status == Status.SUCCESS)
                    {
                        m_logger.Log("INFO:  Read Part Number Successful");
                        //Check if part number matches the part number in the build record
                        if (build.PartNumber.Substring(1, 8).CompareTo(partNumber) == 0)
                        {
                            m_logger.Log("INFO:  Part Numbers Match");
                        }
                        else
                        {
                            m_logger.Log("INFO:  Part Numbes don't match");
                            build.PartNumberMatches = false;
                            if (build.ECUName == "DCU")
                                flashDCUSuccess = false;
                            else
                                flashECMSuccess = false;
                        }
                    }
                    else
                    {
                        m_logger.Log("INFO:  Read Part Number Failure");
                        if (build.ECUName == "DCU")
                            flashDCUSuccess = false;
                        else
                            flashECMSuccess = false;
                    }*/
                }
            }
            if (m_performECMFlash)
            {
                //Check Part Number
                status = Status.ERROR;
                Thread.Sleep(50);
                m_logger.Log("INFO:  Sending Read Part Number Message");
                //retries to allow time for message to respond
                txMessage.Clear();
                txMessage.Add(0x1A);
                txMessage.Add(0xCC);
                List<byte> data = new List<byte>();
                CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                message = CreateECUMessage(txMessage, m_buildData[0].RequestID, m_buildData[0].ResponseID);
                message.m_responseExpected = true;
                bool messageSuccess = m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref data);
                Thread.Sleep(250);
                char[] tempData = new char[8];
                int j = 0;
                //hardcoded length to 6
                for (int i = 2; i < 6; i++)
                {
                    tempData[j] = ((char)(data[i] / 16 + 48));
                    tempData[j + 1] = ((char)(data[i] % 16 + 48));
                    j += 2;
                }
                string partNumber = new string(tempData);
                status = messageSuccess ? Status.SUCCESS : Status.FAILURE;
                if (status == Status.SUCCESS)
                {
                    m_logger.Log("INFO:  Read Part Number Successful");
                    //Check if part number matches the part number in the build record
                    if (m_buildData[0].PartNumber.Substring(1, 8).CompareTo(partNumber) == 0)
                    {
                        m_logger.Log("INFO:  Part Numbers Match");
                    }
                    else
                    {
                        m_logger.Log("INFO:  Part Numbes don't match");
                        m_buildData[0].PartNumberMatches = false;
                        flashECMSuccess = false;
                    }
                }
                else
                {
                    m_logger.Log("INFO:  Read Part Number Failure");
                    flashECMSuccess = false;
                }
                
                //Clear Speed Limit
                m_logger.Log("INFO:  Clearing ECM Speed Limit");
                txMessage.Clear();
                txMessage.Add(0xAE);
                txMessage.Add(0x01);
                txMessage.Add(0x00);
                txMessage.Add(0x40);
                txMessage.Add(0x00);
                txMessage.Add(0x00);
                txMessage.Add(0x00);
                status = SendMessage(m_buildData[0], txMessage, true);

                if (status == Status.SUCCESS)
                {
                    m_logger.Log("INFO:  Clear ECM Speed Limit Success");
                }
                else
                {
                    m_logger.Log("INFO:  Clear ECM Speed Limit Failure");
                    flashECMSuccess = false;
                }
                Thread.Sleep(250);
                //Write Vehicle Speed Value
                m_logger.Log("INFO:  Writing Vehicle Speed Value");
                txMessage.Clear();
                txMessage.Add(0x3B);
                txMessage.Add(0x5A);
                if (m_buildData[3].SpeedMeter.Equals("31D9"))
                {
                    txMessage.Add(0x31);
                    txMessage.Add(0xD9);
                }
                else if (m_buildData[3].SpeedMeter.Equals("31A7"))
                {
                    txMessage.Add(0x31);
                    txMessage.Add(0xA7);
                }
                else if (m_buildData[3].SpeedMeter.Equals("31D3"))
                {
                    txMessage.Add(0x31);
                    txMessage.Add(0xD3);
                }
                else
                {
                    txMessage.Add(0x31);
                    txMessage.Add(0xD9);
                }
                
                txMessage.Add(0xFF);
                txMessage.Add(0xFF);
                status = SendMessage(m_buildData[0], txMessage, true);
                if (status == Status.SUCCESS)
                {
                    m_logger.Log("INFO:  Writing Vehicle Speed Value Success");
                }
                else
                {
                    m_logger.Log("INFO:  Writing Vehicle Speed Value Failure");
                    flashECMSuccess = false;
                }
                Thread.Sleep(250);
            }
            
            if (m_buildData[0].ESNWriteRequired && m_performECMFlash)
            {
                // ECM flash successful and model requires ESN write
                string esn = m_buildData[0].EngineSerialNumber;
                //write ESN
                txMessage.Clear();
                status = Status.ERROR;
                m_logger.Log("INFO:  Sending Write ESN Message");
                txMessage.Add(0x3B);
                txMessage.Add(0x7F);
                //ESN length message must be 6 characters with fill bytes preceded being 0x00
                if (esn.Length < 17)
                {
                    for (int x = esn.Length; x < 17; x++)
                        txMessage.Add(0x00);
                }
                temp = Encoding.ASCII.GetBytes(esn);
                txMessage.AddRange(temp);

                //retries to allow time for ESN message to respond
                status = SendMessage(m_buildData[0], txMessage, true);
                Thread.Sleep(250);
                if (status == Status.SUCCESS)
                {
                    m_logger.Log("INFO:  Write ESN Message Successful");
                    //read ESN
                    txMessage.Clear();
                    m_logger.Log("INFO:  Sending read ESN Message");
                    txMessage.Add(0x1A);
                    txMessage.Add(0x7F);
                    SendMessage(m_buildData[0], txMessage, true);
                }
                else
                {
                    m_logger.Log("INFO:  Write ESN Message Failure");
                    m_buildData[0].SerialNumberWriteSuccess = false;
                    flashECMSuccess = false;
                }
            }
            else
            {
                m_logger.Log("INFO:  No ESN Write Required");
            }
            if (m_performECMFlash)
            {
                if (flashECMSuccess)
                {
                    m_buildData[0].ProgrammingSuccess = true;
                    m_ecmResultColor = Color.Green;
                }
                else
                {
                    m_ecmResultColor = Color.Red;
                }
            }
            if (m_performDCUFlash)
            {
                if (flashDCUSuccess)
                {
                    m_buildData[2].ProgrammingSuccess = true;
                    m_dcuResultColor = Color.Green;
                }
                else
                {
                    m_dcuResultColor = Color.Red;
                }
            }
            
        }
        public void CalibrateMimamori()
        {
            if (m_performMimamoriFlash)
            {
                //customer requested to ignore the part number
                //and make sure the flash station only work with 
                //the CAN communication since the part number
                //might change and they don't want to call us
                //to make the software changes everytime.

                 CalibrateCanMimamori();
              /*  if (Mimamori_Can == mimamoriPartNo)
                {
                    CalibrateCanMimamori();
                }
                else if (Mimamori_Kline == mimamoriPartNo)
                {
                    CalibrateKLinMimamori();
                }*/

            }
        }
        public void CalibrateKLinMimamori()
        {
            m_logger.Log("INFO:  Calibrating the Mimamori Module");
            bool messageSuccess = true;
            CcrtJ2534Defs.ECUMessage setFiltermessage = new CcrtJ2534Defs.ECUMessage();
            List<CcrtJ2534Defs.Response> messageResponses = new List<CcrtJ2534Defs.Response>();
            List<byte> requestID = new List<byte>();
            List<byte> responseID = new List<byte>();
            List<byte> txMessage = new List<byte>();
            ArrayList txMessages = new ArrayList();
            ArrayList messageNames = new ArrayList();
            List<byte> data = new List<byte>();

            //Calibration Message Names and Data
            messageNames.Add("Enter Programming Mode");
            txMessages.Add(new byte[] { 0x82, 0x80, 0xF1, 0x10, 0x85 });
            messageNames.Add("Security Access");
            txMessages.Add(new byte[] { 0x82, 0x80, 0xF1, 0x27, 0x01 });
            messageNames.Add("Read Part Number");
            txMessages.Add(new byte[] { 0x82, 0x80, 0xF1, 0x1A, 0x91 });
            messageNames.Add("Program VIN");
            txMessages.Add(new byte[] { 0x93, 0x80, 0xF1, 0x3B, 0x90 });
            messageNames.Add("Diesel ECM Equipped");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x20, 0x01 });
            messageNames.Add("SRC Equipped");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x22, 0x01 });
            messageNames.Add("Turbo Controller Equipped");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x23, 0x01 });
            messageNames.Add("TCM Equipped");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x24, 0x11 });
            messageNames.Add("ABS Equipped Message");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x29, 0x01 });
            messageNames.Add("Multimeter Equipped");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x2E, 0x01 });
            messageNames.Add("ENG Model Data");
            txMessages.Add(new byte[] { 0x8A, 0x80, 0xF1, 0x3B, 0x40, 0x34, 0x48, 0x4B, 0x31, 0x54, 0x43, 0x53, 0x20 });
            messageNames.Add("T/M Model Data");
            txMessages.Add(new byte[] { 0x8A, 0x80, 0xF1, 0x3B, 0x41 });//, 0x32, 0x35, 0x35, 0x30, 0x52, 0x44, 0x53, 0x20 });
            messageNames.Add("Tire Radius Data");
            txMessages.Add(new byte[] { 0x84, 0x80, 0xF1, 0x3B, 0x42 });//, 0x02, 0x08 });
            messageNames.Add("Final Gear Ratio");
            txMessages.Add(new byte[] { 0x84, 0x80, 0xF1, 0x3B, 0x43 }); //, 0x18, 0x17 });
            messageNames.Add("Vehicle Model Data");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x44, 0x06 });
            messageNames.Add("Electrical Equipment Data");
            txMessages.Add(new byte[] { 0x83, 0x80, 0xF1, 0x3B, 0x45, 0x02 });
            messageNames.Add("Gear Ratio Data");
            txMessages.Add(new byte[] { 0xA8, 0x80, 0xF1, 0x3B, 0x47, 0x06, 0x00, 0x0D, 0xB6, 0x07, 0x6C, 0x05, 0xA0, 0x03, 0xE8, 0x02, 0xE4, 0x02, 0x80, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
            messageNames.Add("Engine Oil Life Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x53, 0x00, 0xF5, 0x8F, 0x48 });
            messageNames.Add("Engine Oil Life Pre-Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x54, 0x00, 0x18, 0x8E, 0x54 });
            messageNames.Add("Starter Life Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x59, 0x00, 0x00, 0x75, 0x30 });
            messageNames.Add("ACG OH Life Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x5B, 0x0E, 0xE6, 0xB2, 0x80 });
            messageNames.Add("Engine Speed ACG OH Life Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x5C, 0x00, 0x0F, 0x42, 0x40 });
            messageNames.Add("T/M OH Warning");
            txMessages.Add(new byte[] { 0x86, 0x80, 0xF1, 0x3B, 0x5D, 0x00, 0x5B, 0x8D, 0x80 });
            messageNames.Add("Security Lock");
            txMessages.Add(new byte[] { 0x82, 0x80, 0xF1, 0x31, 0x08 });
            messageNames.Add("End Diagnostic Session");
            txMessages.Add(new byte[] { 0x81, 0x80, 0xF1, 0x82 });

            m_progressBarRangeMin = 0;
            m_progressBarRangeMax = txMessages.Count;

            txMessage.Clear();
            //StartCommunication message
            txMessage.Add(0x81);
            txMessage.Add(0x80);
            txMessage.Add(0xF1);
            txMessage.Add(0x81);
            setFiltermessage = CreateECUMessage(txMessage, requestID, responseID);
            //FastInitSequence
            CcrtJ2534Device dev = m_vehicleCommInterface.GetCcrtJ2534Device(m_deviceName);
            CcrtJ2534Channel chan = dev.GetChannel(m_mimaChannelName);
            if (!chan.ChannelComm.Connected)
                chan.ChannelComm.Connect();
            ICcrtJ2534ChannelComm comm = chan.ChannelComm;
            bool commInit = comm.PerformFastInitSequence(setFiltermessage, ref messageResponses);

            requestID.Add(0x80);
            requestID.Add(0x80);
            requestID.Add(0xF1);
            m_buildData[3].RequestID = requestID;
            responseID.Add(0x80);
            responseID.Add(0xF1);
            responseID.Add(0x80);
            m_buildData[3].ResponseID = responseID;

            if (commInit)
            {
                Status status = Status.IN_PROGRESS;
                m_logger.Log("INFO:  Fast Init Successful");
                int retries = 25;

                for (int i = 0; i < txMessages.Count; i++)
                {
                    status = Status.IN_PROGRESS;
                    txMessage.Clear();
                    txMessage.AddRange((byte[])txMessages[i]);
                    if (i == 3)//messageNames[i].Equals("Program VIN"))
                    {
                        byte[] temp = Encoding.ASCII.GetBytes(m_buildData[0].VIN);
                        txMessage.AddRange(temp);
                    }
                    else if (i == 11)//messageNames[i].Equals("T/M Model Data"))
                    {
                        m_logger.Log("Transmission: " + m_buildData[3].Transmission);
                        byte[] temp = Encoding.ASCII.GetBytes(m_buildData[3].Transmission);
                        txMessage.AddRange(temp);
                        for (int j = 0; j < (8 - m_buildData[3].Transmission.Length); j++)
                        {
                            txMessage.Add(0x20);
                        }
                    }
                    else if (i == 12)//messageNames[i].Equals("Tire Radius Data"))
                    {
                        m_logger.Log("TireSize: " + m_buildData[3].TireSize);
                        byte[] temp;
                        temp = new byte[] { 0x00, 0x00 };
                        if (m_buildData[3].TireSize.Equals(".520"))
                            temp = new byte[] { 0x02, 0x08 };
                        else if (m_buildData[3].TireSize.Equals(".458"))
                            temp = new byte[] { 0x01, 0xCA };
                        txMessage.AddRange(temp);
                    }
                    else if (i == 13)//messageNames[i].Equals("Final Gear Ratio"))
                    {
                        m_logger.Log("RearAxel: " + m_buildData[3].RearAxel);
                        byte[] temp;
                        temp = new byte[] { 0x18, 0x17 };
                        if (m_buildData[3].RearAxel.Equals("6.143"))
                            temp = new byte[] { 0x17, 0xFF };
                        else if (m_buildData[3].RearAxel.Equals("5.571"))
                            temp = new byte[] { 0x15, 0xC3 };
                        else if (m_buildData[3].RearAxel.Equals("6.167"))//Current
                            temp = new byte[] { 0x18, 0x17 };
                        txMessage.AddRange(temp);
                    }
                    for (int x = 0; x < retries && status != Status.SUCCESS; x++)
                    {
                        status = SendKlineMessage(m_buildData[3], txMessage, responseID, true, ref data);
                        Thread.Sleep(250);
                    }
                    if (status == Status.SUCCESS)
                    {
                        m_logger.Log("INFO:  " + messageNames[i] + " Message Successful");
                        if (i == 2)
                        {
                            char[] tempData = new char[8];
                            int j = 0;
                            //hardcoded length to 6
                            for (int k = 5; k < 13; k++, j++)
                            {
                                tempData[j] = ((char)data[k]);
                                //tempData[k] = ((char)(data[k] / 16 + 48));
                                //tempData[k] = ((char)(data[k] % 16 + 48));
                            }
                            string partNumber = new string(tempData);
                            status = messageSuccess ? Status.SUCCESS : Status.FAILURE;
                            if (status == Status.SUCCESS)
                            {
                                m_logger.Log("INFO:  Read Part Number Successful");
                                //Check if part number matches the part number in the build record
                                //if (build.PartNumber.CompareTo(partNumber) == 0)
                                if (m_buildData[3].PartNumber.Substring(1, 8).CompareTo(partNumber) == 0)
                                {
                                    m_logger.Log("INFO:  Part Numbers Match");
                                    m_buildData[3].PartNumberMatches = true;
                                }
                                else
                                {
                                    m_logger.Log("INFO:  Part Numbes don't match");
                                    m_buildData[3].PartNumberMatches = false;
                                    messageSuccess = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        m_logger.Log("INFO:  " + messageNames[i] + "Message Failure");
                        messageSuccess = false;
                    }
                    m_progressBarValue = i;
                }
                m_progressBarValue = m_progressBarRangeMax;
            }
            else
            {
                m_logger.Log("INFO:  Fast Init Failure");
                messageSuccess = false;
            }
            if (messageSuccess)
            {
                m_buildData[3].ProgrammingSuccess = true;
                m_mimaResultColor = Color.Green;
            }
            else
            {
                m_mimaResultColor = Color.Red;
            }
        }
        public void CalibrateCanMimamori()
        {
            
            List<byte> txMessage = new List<byte>();
            List<byte> responseData = new List<byte>();
            Status status = Status.ERROR;
            
            MimamoriMessagesBuilder mimamoriMessageBuilder = new MimamoriMessagesBuilder();
           
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.CommunicationStart());
          
            //enter programming mode
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.ProgramModeMessage());

            //enter security
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.SecurityAccessMessage()); 

            //can setting
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.CanSettingMessage());
            
            //flash the vin
            txMessage.Clear();
            txMessage.AddRange(mimamoriMessageBuilder.MimamoriModuleVinFlash(m_buildData[0].VIN).RequestMessage);
            m_logger.Log("INFO:  VIn number sent: " + BitConverter.ToString(txMessage.ToArray()));
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.MimamoriModuleVinFlash(m_buildData[0].VIN));

            //T/M Model Data
            m_logger.Log("Transmission: " + m_buildData[3].Transmission);
            status = SendMimamoriCanMessage(mimamoriMessageBuilder.TransmissionModuleModelDataMessage(m_buildData[3].Transmission));
            
            //tire radius data
            m_logger.Log("Tire size: " + m_buildData[3].TireSize);
            if (m_buildData[3].TireSize.Equals(".520"))
                status = SendMimamoriCanMessage(mimamoriMessageBuilder.TireRadiusMessage(520));
            else if (m_buildData[3].TireSize.Equals(".458"))
                status = SendMimamoriCanMessage(mimamoriMessageBuilder.TireRadiusMessage(458));

            //Final Gear Ratio
            m_logger.Log("Final Gear Ratio: " + m_buildData[3].RearAxel);
            if (m_buildData[3].RearAxel.Equals("6.143"))
                status = SendMimamoriCanMessage(mimamoriMessageBuilder.FinalGearRatioMessage(6143));
            else if (m_buildData[3].RearAxel.Equals("5.571"))
                status = SendMimamoriCanMessage(mimamoriMessageBuilder.FinalGearRatioMessage(5571));
            else if (m_buildData[3].RearAxel.Equals("6.167"))//Current
                status = SendMimamoriCanMessage(mimamoriMessageBuilder.FinalGearRatioMessage(6167));

            //do the rest of the messages
            foreach(MimamoriModuleCanMessage message in mimamoriMessageBuilder.getMessageList())
            {
                status = SendMimamoriCanMessage(message);
            }
            
            if (status == Status.SUCCESS)
            {
                m_buildData[3].ProgrammingSuccess = true;
                m_mimaResultColor = Color.Green;
            }
            else
            {
                m_mimaResultColor = Color.Red;
            }
            m_progressBarValue = m_progressBarRangeMax;

        }
        Status SendMimamoriCanMessage( MimamoriModuleCanMessage mimamoriMessage)
        {

            Status status = Status.ERROR;
            List<byte> responseData = new List<byte>();
            List<byte> txMessage = new List<byte>();
            txMessage.AddRange(mimamoriMessage.RequestMessage);
            status = Status.IN_PROGRESS;
            m_progressBarValue++;
           
            CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();

            message = CreateECUMessage(txMessage, MimamoriModuleCanMessage.CanRequestId, MimamoriModuleCanMessage.CanResponseId);
            message.m_responseExpected = true;
            message.m_retries = 1;
            if (txMessage.Count > 4)
            {
                message.m_txTimeout = 200;
            }

            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                    message.m_messageFilter);
            m_logger.Log("INFO:  Sending Message: " + mimamoriMessage.MessageDescription);
            m_logger.Log("INFO:  Message Id: " + BitConverter.ToString(txMessage.ToArray()));
            if (m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, message, ref responseData))
            {
                //compare the received date to determine if the message is sent successfully
                
                m_logger.Log("INFO:  Received: " + BitConverter.ToString(responseData.ToArray()));
                status = Status.SUCCESS;
                bool match = true;
                List<byte>expectedData = mimamoriMessage.ResponseMessage;
                if(responseData.Count < expectedData.Count)
                    match = false;
                for (int i = 0; i < expectedData.Count && match; i++)
                {
                    if (expectedData[i] != responseData[i])
                    {
                        match = false;
                        status = Status.FAILURE;
                    }
                }
                if(status == Status.SUCCESS)
                    m_logger.Log("INFO:  Message sent successfully.");
                else
                    m_logger.Log("INFO:  Message Failure");
                
            }
            else
            {
                m_logger.Log("INFO:  Message Failure");
                status = Status.FAILURE;
            }
            
            return status;

        }
        public string GetESN(string vin, string esnDirectory)
        {
            //m_logger.Log("BUllShitLogStatement");

            string esn = "otherBullshit";
            try{
                //obtain ESN from file
                //m_logger.Log("checking for vin: " + vin);
                DirectoryInfo esnDirInfo = new DirectoryInfo(esnDirectory);

                foreach (FileInfo fi in esnDirInfo.GetFiles())
                {
                    //m_logger.Log("checking file: " + fi.Name);
                    if (fi.Name.Length > 17)
                    {//check if VIN matches
                        if (fi.Name.Substring(0,17) == vin)
                        {//get serial number from file
                            lock (m_esnFileChangeMonitor.DirectoryLock)
                            {
                                // Read the file as one string.
                                System.IO.StreamReader myFile =
                                new System.IO.StreamReader(fi.FullName);
                                esn = myFile.ReadLine();
                                SetESNFault(m_esnFileChangeMonitor.m_esnFault);
                            }
                            if (esn.Length == m_defaultESNLength)
                            {
                                //m_logger.Log("INFO:  ESN Obtained: " + esn);
                                return esn;
                            }
                            else
                            {
                          //      m_logger.Log("ERROR:  ESN incorrect length");
                                return esn;
                            }
                        }
                    }
                    else
                    {
                        //m_logger.Log("ERROR:  File name length error");
                    }
                }
                SetESNFault(m_esnFileChangeMonitor.m_esnFault);
            }   
            catch{
                //m_logger.Log("Something went wrong. Caught an Exception in getESN()");
            }   
            return esn;
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
        public Status SendKlineMessage(ECUBuildData build, List<byte> txMessage, List<byte> sourceAddress, bool responseExpected, ref List<byte> data)
        {
            bool messageSuccess = true;
            CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
            message.m_responseExpected = responseExpected;
            message.m_retries = 1;
            if (txMessage.Count > 4)
            {
                message.m_txTimeout = 200;
            }
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_mimaChannelName,
                    message.m_messageFilter);

            m_logger.Log("INFO:  Sending Message: " + BitConverter.ToString(txMessage.ToArray()));
            if (m_vehicleCommInterface.GetECUData(m_deviceName, m_mimaChannelName, message, ref data))
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
            if (messageSuccess)
            {
                return Status.SUCCESS;
            }
            else
            {
                return Status.FAILURE;
            }
        }
        public Status SendMessage(ECUBuildData build, List<byte> txMessage, bool responseExpected)
        {
            List<byte> data = new List<byte>();
           
            bool messageSuccess = true;
            CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            message = CreateECUMessage(txMessage, build.RequestID, build.ResponseID);
            message.m_responseExpected = responseExpected;
            message.m_retries = 1;
            if (txMessage.Count > 4)
            {
                message.m_txTimeout = 200;
            }
         
            m_vehicleCommInterface.AddMessageFilter(m_deviceName, m_channelName,
                    message.m_messageFilter);

            m_logger.Log("INFO:  Sending Message: " + BitConverter.ToString(txMessage.ToArray()));
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
            if (messageSuccess)
            {
                return Status.SUCCESS;
            }
            else
            {
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
                message.m_retries = 3;
                if (txMessage.Count > 4 || !m_isFlashRequired)
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
        //create message using module response / request id and tx message
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
                else if (!m_dcuThreadComplete && m_performDCUFlash)
                {//ecm completed start from estimated size of ecm and add dcu progress
                    if (m_performECMFlash)
                    {
                        m_progressBarValue = m_ecmDataSizeAdjusted + m_dcuInterpreter.m_opCodeHandler.m_bytesTransmitted;
                        if (m_performTCMFlash)
                        {//tcm completed add estimated size of tcm
                            m_progressBarValue += m_tcmDataSizeAdjusted;
                        }
                    }
                    else
                    {
                        m_progressBarValue = m_dcuInterpreter.m_opCodeHandler.m_bytesTransmitted;
                        if (m_performTCMFlash)
                        {//tcm completed add estimated size of tcm
                            m_progressBarValue += m_tcmDataSizeAdjusted;
                        }
                    }
                }
                else if (!m_tcmThreadComplete && m_performTCMFlash)
                {//ecm completed start from estimated size of ecm and add mimamori progress
                    if (m_performECMFlash)
                    {
                        m_progressBarValue = m_ecmDataSizeAdjusted + m_mimaInterpreter.m_opCodeHandler.m_bytesTransmitted;
                        if (m_performTCMFlash)
                        {//tcm completed add estimated size of tcm
                            m_progressBarValue += m_tcmDataSizeAdjusted;
                        }
                        if (m_performDCUFlash)
                        {//dcu completed add estimated size of dcu
                            m_progressBarValue += m_dcuDataSizeAdjusted;
                        }
                    }
                    else
                    {
                        m_progressBarValue = m_dcuInterpreter.m_opCodeHandler.m_bytesTransmitted;
                        if (m_performTCMFlash)
                        {//tcm completed add estimated size of tcm
                            m_progressBarValue += m_tcmDataSizeAdjusted;
                        }
                        if (m_performDCUFlash)
                        {//dcu completed add estimated size of dcu
                            m_progressBarValue += m_dcuDataSizeAdjusted;
                        }
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
            m_logger.Log("Reporting Flash Results");
            Thread.Sleep(100);
            ReportData();
            m_logger.Log("Flash Results reported");
            Thread.Sleep(100);
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
                            if (!bd.ProgrammingSuccess || !bd.VinWriteSuccess || !bd.SerialNumberWriteSuccess)
                            {
                                pass = false;
                                if (!bd.VinWriteSuccess)
                                {
                                    m_resultText += bd.ECUName + "Vin Write Failed ";
                                }
                                if (!bd.SerialNumberWriteSuccess)
                                {
                                    m_resultText += bd.ECUName + "Serial # Write Failed ";
                                }
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
                    }//if no flash performed, fail
                    if (!pass || (!m_performTCMFlash && !m_performECMFlash && !m_performDCUFlash && !m_performMimamoriFlash))
                    {
                        //programming failed
                        SetPrompt1BGColor(Color.Red);
                        passed = false;
                        m_logger.Log("INFO:  Reporting Fail");
                        resultNode.InnerText = "Fail";
                        //write file to shared folder
                        try
                        {
                            WritePassIndicationFile("FAIL");
                        }
                        catch (Exception e)
                        {
                            m_logger.Log("Could not report data because of exception:" + e.ToString());
                        }
                    }
                    else
                    {
                        //programming passed
                        SetPrompt1(prompt.PASS);
                        SetPrompt1BGColor(Color.Green);
                        passed = true;
                        m_logger.Log("INFO:  Reporting Pass");
                        resultNode.InnerText = "Pass";
                        //write file to shared folder
                        try
                        {
                            WritePassIndicationFile("PASS");
                        }
                        catch(Exception e)
                        {
                            m_logger.Log("Could not report data because of exception:" + e.ToString());
                        }
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
                    FileSystemInfo fileInfo = resultsDirInfo.GetFileSystemInfos().OrderByDescending(fi => fi.CreationTime).First();
                    fileInfo.Delete();
                }
                if (m_flashRetryAttempted)
                {//Display message box to operator
                    m_displayDisconnectBatteryBox = true;
                }
            }

        }
        //Write pass indication to a local folder first
        public void WritePassIndicationFile(string result)
        {
            m_logger.Log("Writing Result to File");
            string filename = m_buildData[0].VIN + ".ECM";
            lock (m_passFileFtp.DirectoryLock)
            {
                using (StreamWriter outfile = new StreamWriter(m_passIndicationLocalDirectory + filename))
                {
                    string timeStamp = (DateTime.Now.ToString("yyyyMMddHHmmss"));
                    outfile.Write(timeStamp+result);
                    outfile.Flush();
                    long numb = outfile.BaseStream.Length;
                    m_logger.Log(".ECM data size check: " + numb.ToString());
                    m_logger.Log("Wrote " + timeStamp + result + " to result file.");
                }
               var length = new System.IO.FileInfo(m_passIndicationLocalDirectory + filename).Length;
               m_logger.Log("ECM file size check: " + length.ToString());
            }
        }
        public void PassFileIndicationTransferThread()
        {
            while (!m_terminateThreads)
            {
                lock (m_passFileFtp.DirectoryLock)
                {//copy files over
                    string[] files = System.IO.Directory.GetFiles(m_passIndicationLocalDirectory);
                    try
                    {
                        if (files.Count() > 0)
                        {
                            if (m_passFileFtp.UploadToSource(files.ToList()))
                            {
                                m_logger.Log("INFO:  Pass file Transmit Successful");
                            }
                            else
                            {
                                m_logger.Log("INFO:  Pass file Transmit Error, Retransmitting...");
                            }
                        }
                    }
                    catch
                    {
                        m_logger.Log("ERROR:  Pass Result File Transfer Error");
                    }
                }
                Thread.Sleep(10000);
            }
        }
        public void CreateCableConnectCheckMessage()
        {
            List<byte> txMessage = new List<byte>();
            txMessage.Clear();
            txMessage.Add(0x1A);
            txMessage.Add(0x90);
            //txMessage.Add(0x1A);
            //txMessage.Add(0xB0);
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
            if (m_currentState != StateName.FLASH_ECUS)
            {
                if (!m_usingCableConnectForced)
                {
                    if (m_cableConnectMessage == null)
                    {
                        CreateCableConnectCheckMessage();
                    }
                    List<byte> data = new List<byte>();
                    //bool temp = m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, m_cableConnectMessage, ref data);
                    //m_logger.Log("INFO:  IsCableConnected() Received: " + BitConverter.ToString(data.ToArray()));
                    return m_vehicleCommInterface.GetECUData(m_deviceName, m_channelName, m_cableConnectMessage, ref data); ;
                }
                else
                {
                    return m_cableConnectForced;
                }

            }
            else
                return true;
        }
        public bool ReconnectCurrentDevice()
        {
            //Check if still connected
            bool success = IsCableConnected();
            if (!success)
            {   //Not connected, start reconnect
                //Disconnect current channel
                CcrtJ2534Device dev = m_vehicleCommInterface.GetCcrtJ2534Device(m_deviceName);
                CcrtJ2534Channel chan = dev.GetChannel(m_channelName);
                chan.ChannelComm.Disconnect();
                chan.ChannelComm.Connected = false;
                //Check for current device to connect to
                m_vehicleCommInterface.PopulateVehicleCommChannels();
                //Check if device has reconnected
                success = IsCableConnected();
            }
            return success;
        }
        public bool ValidBuildDataReceived()
        {
            return m_buildDataValid;
        }
        public bool HaveESNFault()
        {
            return m_esnFault;
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
        public void SetESNFault(bool faulted)
        {
            m_esnFault = faulted;
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
            m_buildDataFcm.StopFileMonitorThread();
            m_esnFileChangeMonitor.StopFileMonitorThread();
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
        public Color GetDCUResultBGColor()
        {
            return m_dcuResultColor;
        }
        public Color GetMimamoriResultBGColor()
        {
            return m_mimaResultColor;
        }
        public Color GetBrakeApplySensorResultBGColor()
        {
            return m_brakeApplySensorResultColor;
        }
        public void SetPerformECMFlash(bool perform)
        {
            m_performECMFlash = perform;
            //if (m_buildData.Count == 2)
            if (m_buildData.Count == 4 || m_buildData.Count == 2)
            {//build data loaded - modify
                m_buildData[0].PerformFlash = perform;
            }
        }
        public void SetPerformTCMFlash(bool perform)
        {
            m_performTCMFlash = perform;
            //if (m_buildData.Count == 2)
            if (m_buildData.Count == 4 || m_buildData.Count == 2)
            {//build data loaded - modify
                m_buildData[1].PerformFlash = perform;
            }
        }
        public void SetPerformDCUFlash(bool perform)
        {
            m_performDCUFlash = perform;
            if (m_buildData.Count == 4)
            {//build data loaded - modify
                m_buildData[2].PerformFlash = perform;
            }
        }
        public void SetPerformMimamoriFlash(bool perform)
        {
            m_performMimamoriFlash = perform;
            if (m_buildData.Count == 4)
            {//build data loaded - modify
                m_buildData[3].PerformFlash = perform;
            }
        }
        public void SetPerformBASLearn(bool perform)
        {
            m_performBASLearn = perform;
        }
        public void SetDisplayDisconnectBatteryBox(bool display)
        {
            m_displayDisconnectBatteryBox = display;
        }
        public bool GetDisplayDisconnectBatteryBox()
        {
            return m_displayDisconnectBatteryBox;
        }
        public void SetFlashRequired(bool flashRequired)
        {
            m_isFlashRequired =  flashRequired;
            if (!m_isFlashRequired)
            {
                m_ecuNames.Add("DCU");
                m_ecuNames.Add("Mimamori");
                System.Windows.Forms.RichTextBox dcuRTB = new System.Windows.Forms.RichTextBox();
                m_ecmLogger = new Logger(dcuRTB, "DCULog", m_logsDirectory);
                System.Windows.Forms.RichTextBox mimaRTB = new System.Windows.Forms.RichTextBox();
                m_tcmLogger = new Logger(mimaRTB, "MimamoriLog", m_logsDirectory);
                m_performDCUFlash = true;
                m_performMimamoriFlash = true;
            }
        }
        public void SetDefaultESNLength(int length)
        {
            m_defaultESNLength = length;
        }

        /// <summary>
        /// Main Form Model Member Variables
        /// </summary>

        /// <summary>
        /// Current flash status
        /// </summary>
        private volatile Status m_status;

        /// <summary>
        /// Prompt 1 text
        /// </summary>
        private string m_prompt1;

        /// <summary>
        /// Prompt 2 text
        /// </summary>
        private string m_prompt2;

        /// <summary>
        /// Color to set prompt1
        /// </summary>
        private Color m_prompt1BGColor;

        /// <summary>
        /// Color to set prompt2
        /// </summary>
        private Color m_prompt2BGColor;

        /// <summary>
        /// Color to set ecm result box
        /// </summary>
        private Color m_ecmResultColor;

        /// <summary>
        /// Color to set tcm result box
        /// </summary>
        private Color m_tcmResultColor;

        /// <summary>
        /// Color to set ecm result box
        /// </summary>
        private Color m_dcuResultColor;

        /// <summary>
        /// Color to set tcm result box
        /// </summary>
        private Color m_mimaResultColor;

        /// <summary>
        /// Color to set tcm result box
        /// </summary>
        private Color m_brakeApplySensorResultColor;

        /// <summary>
        /// Current scanned barcode
        /// </summary>
        private string m_barcode;

        /// <summary>
        /// Interface to vehicle comm device
        /// </summary>
        private IVehicleCommServerInterface m_vehicleCommInterface;

        /// <summary>
        /// Input server interface for barcode device settings
        /// </summary>
        private IInputServerInterface m_inputServerInterface;

        /// <summary>
        /// Name of barcode input device
        /// </summary>
        private string m_barcodeInputDevice;
        
        /// <summary>
        /// Name of Vehilce comm device
        /// </summary>
        private string m_deviceName;
        
        /// <summary>
        /// Name of Vehilce comm channel
        /// </summary>
        private string m_channelName;

        /// <summary>
        /// Name of Vehilce comm channel
        /// </summary>
        private string m_mimaChannelName;

        /// <summary>
        /// flag indicating status of build data retrieved
        /// </summary>
        private bool m_buildDataValid;

        /// <summary>
        /// flag indicating status of ESN Data transfer
        /// </summary>
        private bool m_esnFault;

        /// <summary>
        /// Current flash step running ie connect cable
        /// </summary>
        private StateName m_currentState;

        /// <summary>
        /// flag indicated a retry has been attempted
        /// </summary>
        public bool m_flashRetryAttempted;

        /// <summary>
        /// Collection of build data for each ecu
        /// </summary>
        private List<ECUBuildData> m_buildData;

        /// <summary>
        /// Name used to identify ECU being flash - BEP assigned
        /// </summary>
        private List<String> m_ecuNames;

        private CcrtJ2534Defs.ECUMessage m_keyOnMessage;
        
        /// <summary>
        /// Main Flash station Logger
        /// </summary>
        public Logger m_logger;

        /// <summary>
        /// Count of the number of seconds the brake pedal is not applied
        /// </summary>
        public bool m_keyOffFlag;

        /// <summary>
        /// Count of the number of seconds the brake pedal is not applied
        /// </summary>
        public bool m_brakeRelearnPassed;

        /// <summary>
        /// Count of the number of seconds the brake pedal is not applied
        /// </summary>
        public int m_brakeReleasedCounter;

        /// <summary>
        /// Boolean if the brake is still released
        /// </summary>
        public bool m_brakeReleased;

        /// <summary>
        /// Boolean if the two minute BAS relearn wait has started
        /// </summary>
        public bool m_keyOffEngineOffWaitStart;

        /// <summary>
        /// Constant value - Index of pn where file name begins
        /// </summary>
        public const int FILE_NAME_PN_START_INDEX = 1;

        /// <summary>
        /// Constant value - length of calibration files begining with aN "8"
        /// </summary>
        public const int CAL_FILE_NAME_LENGTH_BEGINNING_WITH_EIGHT = 8;

        /// <summary>
        /// Constant value - length of calibration files begining with a "1"
        /// </summary>
        public const int CAL_FILE_NAME_LENGTH_BEGINNING_WITH_ONE = 10;

        /// <summary>
        /// Constant value - default length of utility file names
        /// </summary>
        public const int UTILITY_FILE_NAME_LENGTH = 8;
        
        /// <summary>
        /// Constant value - maximum number of softare part numbers
        /// </summary>        
        public const int MAX_NUM_SOFTWARE_PART_NOS = 20;

        /// <summary>
        /// Constant value - Length of barcode scan
        /// </summary> 
        public const int BARCODE_LENGTH = 30;

        /// <summary>
        /// Constant value - delay between keep alive messages
        /// </summary>
        public const int TESTER_PRESENT_DELAY = 1000;

        /// <summary>
        /// Constant value - number of retries allowed in a given flash process
        /// </summary>
        public const int FLASH_RETRIES = 1;

        /// <summary>
        /// Flag used to command threads to terminate
        /// </summary>
        private volatile bool m_terminateThreads;

        /// <summary>
        /// Flag used to stop tester present thread
        /// </summary>
        private volatile bool m_stopTesterPresentThread;

        /// <summary>
        /// Flag used to stop progress bar thread
        /// </summary>
        private volatile bool m_stopProgressBarThread;

        /// <summary>
        /// Flag indicating If user selected to force cable connect on
        /// </summary>
        private volatile bool m_cableConnectForced;

        /// <summary>
        /// Flag indicating If debug force cable connect is being used
        /// </summary>
        private volatile bool m_usingCableConnectForced;

        /// <summary>
        /// Flag indicating ECM flash thread complete
        /// </summary>
        private volatile bool m_ecmThreadComplete;

        /// <summary>
        /// Flag indicating TCM flash thread complete
        /// </summary>
        private volatile bool m_tcmThreadComplete;

        /// <summary>
        /// Flag indicating ECM flash thread complete
        /// </summary>
        private volatile bool m_dcuThreadComplete;

        /// <summary>
        /// Flag indicating TCM flash thread complete
        /// </summary>
        private volatile bool m_mimaThreadComplete;

        /// <summary>
        /// Flag used to command view to display instructional battery disconnect message
        /// </summary>
        private volatile bool m_displayDisconnectBatteryBox;

        /// <summary>
        /// Current Progress bar value
        /// </summary>
        private volatile int m_progressBarValue;
        
        /// <summary>
        /// Min Progress bar value
        /// </summary>
        private volatile int m_progressBarRangeMin;
        
        /// <summary>
        /// Max Progress bar value
        /// </summary>
        private volatile int m_progressBarRangeMax;

        /// <summary>
        /// Estimated Data size to be transfered to ECM
        /// </summary>
        private volatile int m_ecmDataSizeAdjusted;

        /// <summary>
        /// Estimated Data size to be transfered to TCM
        /// </summary>
        private volatile int m_tcmDataSizeAdjusted;

        /// <summary>
        /// Estimated Data size to be transfered to DCU
        /// </summary>
        private volatile int m_dcuDataSizeAdjusted;

        /// <summary>
        /// Estimated Data size to be transfered to Mimamori
        /// </summary>
        private volatile int m_mimaDataSizeAdjusted;

        /// <summary>
        /// Flag which enables step through debugging of utility file instructions
        /// </summary>
        private volatile bool m_debugMode = false;

        /// <summary>
        /// Seperate log for ECM flash process details
        /// </summary>
        private Logger m_ecmLogger;

        /// <summary>
        /// Seperate log for TCM flash process details
        /// </summary>
        private Logger m_tcmLogger;

        /// <summary>
        /// Seperate log for ECM flash process details
        /// </summary>
        /*private Logger m_dcuLogger;*/

        /// <summary>
        /// Seperate log for TCM flash process details
        /// </summary>
       /* private Logger m_mimaLogger;*/

        /// <summary>
        /// Keep alive message to be sent throughout flash process
        /// </summary>
        private List<CcrtJ2534Defs.ECUMessage> m_testerPresentMessages;
        
        /// <summary>
        /// Message used to determine cable connect status
        /// </summary>
        private J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage m_cableConnectMessage;
        
        /// <summary>
        /// Build data for current scanned vehicle
        /// </summary>
        private XmlDocument m_currentBuild;
        
        /// <summary>
        /// File Change monitor for Build data on DVT
        /// </summary>
        private FtpFileMonitor m_buildDataFcm;
        
        /// <summary>
        /// File Change monitor for ESN File on DVT
        /// </summary>
        private FtpFileMonitor m_esnFileChangeMonitor;

        /// <summary>
        /// ftp object used to upload pass files
        /// </summary>
        private FtpFileMonitor m_passFileFtp;

        /// <summary>
        /// Interpreter for ECM utility file
        /// </summary>
        private Interpreter m_ecmInterpreter;
        
        /// <summary>
        /// Interpreter for TCM utility file
        /// </summary>
        private Interpreter m_tcmInterpreter;

        /// <summary>
        /// Interpreter for DCU utility file
        /// </summary>
        private Interpreter m_dcuInterpreter;

        /// <summary>
        /// Interpreter for Mimamori utility file
        /// </summary>
        private Interpreter m_mimaInterpreter;
        
        /// <summary>
        /// Serial port used for barcode read.
        /// </summary>
        private SerialPortLibrary.CcrtSerialPort m_serialPort;
        
        /// <summary>
        /// boolean option to perform TCM Flash Process.
        /// </summary>
        private bool m_performTCMFlash;
        
        /// <summary>
        /// boolean option to perform ECM Flash Process.
        /// </summary>
        private bool m_performECMFlash;

        /// <summary>
        /// boolean option to perform DCU Flash Process.
        /// </summary>
        private bool m_performDCUFlash;

        /// <summary>
        /// boolean option to perform Mimamori Flash Process.
        /// </summary>
        private bool m_performMimamoriFlash;

        /// <summary>
        /// boolean option to perform BAS Relearn Process.
        /// </summary>
        private bool m_performBASLearn;
        
        
        /// <summary>
        /// Model code of current scanned Vehicle.
        /// </summary>
        private string m_modelCode;

        /// <summary>
        /// boolean option if the Brake Apply Sensor Home Position has been learned.
        /// </summary>
        private bool m_basHomePositionLearned;

        private bool m_isFlashRequired;

        private int m_defaultESNLength;

        //To do Make Configurable
        private string m_flashFileDirectory = @"C:\\FlashStation\\CalFiles\\";
        private string m_buildFileDirectory = @"C:\\FlashStation\\BuildFiles\\";
        private string m_esnDirectory = @"C:\\FlashStation\\ESN\\";
        private string m_tempBuildFileDirectory = @"C:\\FlashStation\\TempBuildFiles\\";
        private string m_tempESNDirectory = @"C:\\FlashStation\\TempESNFiles\\";
        private string m_resultsDirectory = @"C:\\FlashStation\\Results\\";
        private string m_logsDirectory = @"C:\\FlashStation\\Logs\\";
        private string m_passIndicationLocalDirectory = @"C:\\FlashStation\\TransferFiles\\";

        //Plant 12 info
        private string m_userLogin = "burke";
        private string m_password = "porter";
        private string m_ftpServerIp = "172.16.253.1";
        //Plant 5 info
        //private string m_userLogin = "ccrtfp";
        //private string m_password = "ccrtfp";
        //private string m_userLogin = "burke";
        //private string m_password = "porter";
        //private string m_ftpServerIp = "192.168.1.3";

        private string m_remoteBuildFileLocation = "/TestResults/ftpOutbox/BuildRecords/";
        private string m_remoteESNLocation = "/TestResults/ftpOutbox/ESN/";
        private string m_passIndicationLocation = "/TestResults/PassConfirmation/";

        /// <summary>
        /// Constant value - max result files to store
        /// </summary>
        public const int MAX_RESULT_FILES = 1000;
    }
}
