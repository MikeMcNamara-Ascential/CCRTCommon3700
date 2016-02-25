using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Common.Lib.Interfaces;
using System.Threading;
using Common.Classes;
using LoggingInterface;

namespace Common.Lib.Presenters
{
    public class MainFormDataPresenter : MainFormPresenter<IMainFormView>
    {
        StateName DEFAULT_STATE = StateName.WAIT_FOR_USER_INPUT;
        StateName FINAL_STATE = StateName.WAIT_FOR_CABLEDISCONNECT;
        Thread taskThread;
        Thread taskThread2;
        private Logger m_logger;
        
        private volatile bool m_terminateThreads;

        /// <summary>
        /// Create a new Presenter responsible for coordinating Model and View
        /// </summary>
        /// <param name="view">View form interface</param>
        /// <param name="logger">logging interface for form display and file output</param>
        public MainFormDataPresenter(IMainFormView view,  Logger logger)
            : base(view)
        {
            m_logger = logger;
            MainFormModel.SetLogger(m_logger);
            m_terminateThreads = false;
            //Start bg view update thread
            ThreadStart taskDelegate2 = new ThreadStart(UpdateViewTaskThread);
            taskThread2 = new Thread(taskDelegate2);

            //Start Main Thread
            ThreadStart taskDelegate = new ThreadStart(MainTaskThread);
            taskThread = new Thread(taskDelegate);
        }

        public void StartThreads()
        {
            taskThread2.Start();
            taskThread.Start();
        }
        private volatile StateName m_currentState;
        public void MainTaskThread()
        {//Obtain user input for build data selection
            try
            {
                m_currentState = DEFAULT_STATE;
                //initialize status
                SetStatus(Status.IN_PROGRESS);
                while (MainFormModel.GetStatus() != Status.IN_PROGRESS)
                {
                    Thread.Sleep(100);
                }
                m_logger.Log("INFO:  Main Task Thread Started");
                while (!m_terminateThreads)
                {

                    if (MainFormModel.GetStatus() != Status.TERMINATE &&
                        MainFormModel.GetStatus() != Status.ABORT)
                    {
                        if (m_currentState == DEFAULT_STATE)
                        {//force garbage collection
                            GC.Collect();
                            GC.WaitForPendingFinalizers();
                        }
                        SetStatus(Status.IN_PROGRESS);
                        MainFormModel.CommandState(m_currentState);
                        m_logger.Log("INFO:  Commanded State: " + m_currentState.ToString());
                        //Wait for state to complete
                        while (MainFormModel.GetStatus() == Status.IN_PROGRESS)
                        {
                            Thread.Sleep(100);
                        }
                    }
                    else
                    {
                        m_logger.Log("INFO:  Abort Called Not Commanding State: " + m_currentState.ToString());
                    }
                    //if status is success move on
                    if (MainFormModel.GetStatus() == Status.SUCCESS)
                    {//state specific processing
                        if ((m_currentState == StateName.OBTAIN_BUILD_DATA) && MainFormModel.ValidBuildDataReceived())
                        {
                            MainFormView.DisplayBuildData(MainFormModel.GetBarcode());
                        }
                        // move on to next state
                        m_currentState++;
                        if (m_currentState > FINAL_STATE)
                        {
                            MainFormView.ClearLogDisplay();
                            m_currentState = DEFAULT_STATE;
                        }

                    }
                    else
                    {//Abort, Terminate or Failure Occured
                        if ((MainFormModel.GetStatus() != Status.ABORT) &&
                            (MainFormModel.GetStatus() != Status.TERMINATE) &&
                            (m_currentState == StateName.FLASH_ECUS))
                        {//flash programming failure - goto next state to report data
                            m_currentState++;
                        }
                        else if (MainFormModel.GetStatus() == Status.ABORT)
                        {// wait for program to get through abort process

                            if (m_currentState > StateName.WAIT_FOR_CABLECONNECT)
                            {//report data and continue with vehicle disconnect
                                while (MainFormModel.GetStatus() != Status.IN_PROGRESS && !m_terminateThreads)
                                {
                                    Thread.Sleep(100);
                                }
                                m_currentState = StateName.REPORT_DATA;
                            }
                            else
                            {//allow time for user to see abort
                                Thread.Sleep(10000);
                                SetStatus(Status.IN_PROGRESS);
                                m_currentState = DEFAULT_STATE;
                                MainFormView.ClearLogDisplay();
                            }
                        }
                        else
                        {
                            break;
                        }

                    }
                }
            }
            catch
            {//ignore form closing error
            }
        }
        public void UpdateViewTaskThread()
        {
            m_logger.Log("INFO:  View Update Task Thread Started");
            int pBarRangeMin = 0;
            int pBarRangeMax = 0;
            int pBarValue = 0;
            while (!m_terminateThreads)
                {
                    try
                    {

                        if (m_currentState != StateName.FLASH_ECUS)
                        {
                            MainFormView.CableConnected(MainFormModel.IsCableConnected());
                        }
                        if (pBarRangeMin != MainFormModel.GetProgressBarRangeMin() ||
                            pBarRangeMax != MainFormModel.GetProgressBarRangeMax())
                        {//update progress bar range
                            pBarRangeMax = MainFormModel.GetProgressBarRangeMax();
                            pBarRangeMin = MainFormModel.GetProgressBarRangeMin();
                            MainFormView.SetProgressBarRange(pBarRangeMin, pBarRangeMax);
                        }
                        if (pBarValue != MainFormModel.GetProgressBarValue())
                        {//update progress bar value
                            pBarValue = MainFormModel.GetProgressBarValue();
                            MainFormView.SetProgressBarValue(pBarValue);
                        }

                        pBarRangeMin = MainFormModel.GetProgressBarRangeMin();
                        pBarRangeMax = MainFormModel.GetProgressBarRangeMax();
                        MainFormView.BuildDataValid(MainFormModel.ValidBuildDataReceived());
                        MainFormView.SetBarcodeText(MainFormModel.GetBarcode());
                        MainFormView.SetPrompt1Text(MainFormModel.GetPrompt1());
                        MainFormView.SetPrompt2Text(MainFormModel.GetPrompt2());
                        MainFormView.SetPrompt1BGColor(MainFormModel.GetPrompt1BGColor());
                        MainFormView.SetPrompt2BGColor(MainFormModel.GetPrompt2BGColor());
                        MainFormView.SetECMResultBoxBGColor(MainFormModel.GetECMResultBGColor());
                        MainFormView.SetTCMResultBoxBGColor(MainFormModel.GetTCMResultBGColor());
                        MainFormView.SetDCUResultBoxBGColor(MainFormModel.GetDCUResultBGColor());
                        MainFormView.SetMimamoriResultBoxBGColor(MainFormModel.GetMimamoriResultBGColor());
                        if (MainFormModel.GetDisplayDisconnectBatteryBox())
                        {
                            MainFormView.DisplayTimedDialogBox("Flash Retest Conducted", "Perform Battery Disconnect Procedure", "Ok", 10000);
                            MainFormModel.SetDisplayDisconnectBatteryBox(false);
                        }
                        if (MainFormModel.GetBASLearnWaitStart())
                        {
                            MainFormView.StartBASTimer();
                        }
                        else
                        {
                            MainFormView.StopBASTimer();
                        }
                    }
                    catch (ObjectDisposedException ex)
                    {//ignore form closing
                    }

                    Thread.Sleep(500);
                }
        }

        public string GetBarcodeValue()
        {
            return MainFormModel.GetBarcode();
        }

        public void SubmitBarcodeValue(string barcodeValue)
        {
            MainFormModel.SetBarcode(barcodeValue);
        }
        public List<string> GetAvailableDevices()
        {
            return MainFormModel.GetAvailableJ2534Devices();
        }
        public List<string> GetDeviceChannels(string deviceName)
        {
            return MainFormModel.GetAvailableChannels(deviceName);
        }
        public List<string> GetAvailableBarcodeInputDevices()
        {
            return MainFormModel.GetAvailableBarcodeInputDevices();
        }
        public string GetCurrentDevice()
        {
            return MainFormModel.GetDeviceName();
        }
        public string GetCurrentChannel()
        {
            return MainFormModel.GetChannelName();
        }
        public string GetCurrentBarcodeInputDevice()
        {
            return MainFormModel.GetBarcodeInputDeviceName();
        }
        public string GetBASLearnWaitStart()
        {
            return MainFormModel.GetBarcodeInputDeviceName();
        }
        public void SetDeviceAndChannel(string device, string channel)
        {
            MainFormModel.SetChannelName(channel);
            MainFormModel.SetDeviceName(device);
        }
        public void SetBarcodeInputDevice(string device)
        {
            MainFormModel.SetBarcodeInputDevice(device);
        }
        public void OpenVehicleCommSettings()
        {
            MainFormModel.OpenVehicleCommSettings();
        }
        public void OpenSerialDeviceCommSettings()
        {
            MainFormModel.OpenSerialDeviceCommSettings();
        }
        public void Terminate()
        {
            m_terminateThreads = true;
            MainFormModel.Terminate();
        }
        public void ClearBuildData()
        {//if we are waiting for cable connect, clear build data by setting the status to abort
            if (m_currentState == StateName.WAIT_FOR_CABLECONNECT)
            {
                UserAbort(true);
            }
            else
            {
                m_logger.Log("INFO:  Cannot clear build info in current state");
            }
        }
        public void UserAbort(bool clearBuild)
        {
            if (m_currentState > StateName.WAIT_FOR_USER_INPUT &&
                m_currentState < StateName.REPORT_DATA)
            {
                m_logger.Log("INFO:  User Aborted");
                MainFormModel.SetStatus(Status.ABORT);
                Prompt prompt = new Prompt();

                if (clearBuild || (m_currentState < StateName.FLASH_ECUS))
                {
                    MainFormModel.SetPrompt1("Clearing Build Data");
                    MainFormModel.SetPrompt2("");
                    MainFormView.ClearBuildData();
                }
                else
                {//report aborted result
                    MainFormModel.SetPrompt1(prompt.ABORT_USER1);
                    MainFormModel.SetPrompt2("");
                    MainFormModel.SetPrompt1BGColor(System.Drawing.Color.Yellow);
                    Thread.Sleep(100);
                    if (m_currentState > StateName.WAIT_FOR_CABLECONNECT)
                    {
                        SetStatus(Status.IN_PROGRESS);
                    }
                    else
                    {
                        m_logger.Log("INFO:  Not Reporting results: did not begin flash process");
                    }
                }
            }
            else
            {
                m_logger.Log("INFO:  Cannot Abort in Current State");
            }
        }
        public void SetStatus(Status status)
        {
            MainFormModel.SetStatus(status);
        }
        public void UpdateAvailableBarcodeInputDevices()
        {
            MainFormModel.UpdateAvailableBarcodeInputDevices();
        }
        public List<ECUBuildData> GetBuildData()
        {
            return MainFormModel.GetBuildData();
        }
        public void ToggleForceCableConnect()
        {
            MainFormModel.ToggleForcedCableConnect();
        }
        public void ToggleUsingForceCableConnect()
        {
            MainFormModel.ToggleUsingForcedCableConnect();
        }
        public void SetPerformTCMFlash(bool perform)
        {
            MainFormModel.SetPerformTCMFlash(perform);
        }
        public void SetPerformECMFlash(bool perform)
        {
            MainFormModel.SetPerformECMFlash(perform);
        }
        public void SetPerformDCUFlash(bool perform)
        {
            MainFormModel.SetPerformDCUFlash(perform);
        }
        public void SetPerformMimamoriFlash(bool perform)
        {
            MainFormModel.SetPerformMimamoriFlash(perform);
        }
        public void IncrementBASTimerCount()
        {
            MainFormModel.IncrementBASTimerCount();
        }
        public void ResetBASTimerCount()
        {
            MainFormModel.ResetBASTimerCount();
        }
        public int GetBASTimerCount()
        {
            return MainFormModel.GetBASTimerCount();
        }
        public bool GetBrakePedalReleased()
        {
            return MainFormModel.GetBrakePedalReleased();
        }

    }
}
