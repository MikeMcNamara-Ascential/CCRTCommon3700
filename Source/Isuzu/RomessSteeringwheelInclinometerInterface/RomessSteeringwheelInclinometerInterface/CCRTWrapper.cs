using QnxCcrtInterface;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using System.Net;
using System.Net.Sockets;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using Logger;

namespace RomessSteeringwheelInclinometerInterface
{
    public class CCRTWrapper
    {
        private string qnxip;
        private string windowsip;
        private string qnxport;
        private CcrtInterface QnxCcrt;
        //Let other objects check if there is a valid CcrtInterface yet. Is true after we have conencted to CCRT for the first time.
        private bool initialized;
        private CcrtLogger logger;
        private bool isClosing;
        private System.Threading.Thread ConnectionThread { get; set; }


        /// <summary>
        /// Thread to use for creating the QNX CCRT Interface object.
        /// </summary>
        private System.Threading.Thread CreateQnxInterfaceThread { get; set; }

        public bool Initialized
        {
            get { return initialized; }
            set { initialized = value; }
        }
        public CcrtInterface QNXCCRT
        {
            get { return QnxCcrt; }
            set { QnxCcrt = value; }
        }
        public string QNXIP
        {
            get { return qnxip; }
            set { qnxip = value; }
        }
        public string WindowsIP
        {
            get { return windowsip; }
            set { windowsip = value; }
        }
        public string QNXPort
        {
            get { return qnxport; }
            set { qnxport = value; }
        }
        public CcrtLogger Logger {
            get { return logger; }
            set { logger = value; } 
        }
        public bool IsClosing
        {
            get { return isClosing; }
            set { isClosing = value; }
        }

        public CCRTWrapper()
        {
            initialized = false;
            // Create a new logging object
            DirectoryInfo dir = new DirectoryInfo(".");
            //string filePath = System.IO.Path.Combine(dir.FullName);
            Logger = new CcrtLogger(dir.FullName, "BluetoothReader", 10, 50000);
            CreateQnxInterfaceThread = new System.Threading.Thread(CreateQnxInterface);
            CreateQnxInterfaceThread.Start();
            Logger.Log("Started Stuff");
        }

        public void CreateQnxInterface()
        {   // Make sure this is a valid IP Address
            Int32 attempts = 3;
            Boolean goodIpAddress = false;
            // Set the default IP Address
            //SetQnxIpAddress(QnxIpAddress);
            System.Threading.Thread.Sleep(2000);
            QNXCCRT = new CcrtInterface(IPAddress.Parse(QNXIP), Int32.Parse(QNXPort), Logger);
            //timer3.Start();
            Logger.Log("Created new QNX CCRT Interface");
            do
            {
                try
                {
                    Ping ping = new Ping();
                    PingReply pingResult = ping.Send(IPAddress.Parse(QNXIP));
                    if (pingResult.Status != IPStatus.Success)
                    {   // Could not detect QNX server, allow user to enter new IP Address
                        //IpAddressForm ipForm = new IpAddressForm();
                        //ipForm.ShowDialog();
                        //QnxCcrt.ChangeIpAddress(IPAddress.Parse(CcrtFloorInterface.Properties.Settings.Default.WindowsIpAddress));
                    }
                    else if (pingResult.Status == IPStatus.Success)
                    {   // Found a possible QNX server
                        //SetQnxConnectionStatus(Color.Yellow);
                        Logger.Log("Detected QNX server at " + QNXIP);
                        goodIpAddress = true;
                    }
                    else
                    {
                        //SetQnxConnectionStatus(Color.Red);
                        Logger.Log("Could not detect QNX server at " + QNXIP);
                    }
                }
                catch
                {
                    //SetQnxConnectionStatus(Color.Red);
                    Logger.Log("No network connected!");
                }
            } while (!goodIpAddress && (attempts-- > 0) && !IsClosing);
            // Always start the connection status monitor thread
            // Make sure to kill off the existing connection thread
            if (ConnectionThread != null)
            {
                ConnectionThread.Abort();
            }
            // Create a new thread to attempt to connect to CCRT
            ConnectionThread = new System.Threading.Thread(QnxCcrtConnection);
            ConnectionThread.Start();
        }

        /// <summary>
        /// Attempt to connect to the QNX CCRT System.
        /// </summary>
        public void QnxCcrtConnection()
        {
            while (!QnxCcrt.QnxConnected && !IsClosing) System.Threading.Thread.Sleep(1000);
            if (QnxCcrt.QnxConnected)
            {
                Initialized = true;
                bool lastConnected = QnxCcrt.QnxConnected;

                Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
                {
                    ((MainWindow)Application.Current.MainWindow).CCRTConnected = QnxCcrt.QnxConnected;
                }));
                while (!IsClosing)
                {
                    Logger.Log("Checking Connection: "+ ((QnxCcrt.QnxConnected)?"Connected":"Disconnected"));
                    if (lastConnected != QnxCcrt.QnxConnected)
                    {
                        lastConnected = QnxCcrt.QnxConnected;
                        Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate
                        {
                            ((MainWindow)Application.Current.MainWindow).CCRTConnected = QnxCcrt.QnxConnected;
                        }));
                    }
                    System.Threading.Thread.Sleep(1000);
                }
            }
        }

        public void WriteDataToCCRT(string measuredValue)
        {
            //if(
            QnxCcrt.Write("SteeringWheelAngle", measuredValue);
        }
        public void CCRTReconnect()
        {
            if (Initialized && !QNXCCRT.QnxConnected)
                QNXCCRT.ReconnectSocket();
        }

    }
}
