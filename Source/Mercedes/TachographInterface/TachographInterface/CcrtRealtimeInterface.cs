using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;
using Tach;


namespace TachographInterface
{
    class CcrtRealtimeInterface
    {
        public CcrtRealtimeInterface(System.Windows.Forms.TextBox logTextBox)
        {
            m_logTextBox = logTextBox;
            LogMessage("Created a new CCRT Server interface\r\n");
            // Load the configuration and initialize the interface
            Initialize();
            // Kick off a thread to listen for messages
            StartListenerThread();
        }

        // Allows the new threads to log messages as well as the main thread.
        delegate void SetTextCallback(string text);
        public void LogMessage(string text)
        {
            if (m_logTextBox.InvokeRequired)
            {
                SetTextCallback callBack = new SetTextCallback(LogMessage);
                m_logTextBox.Invoke(callBack, new object[] { text });
            }
            else
            {
                m_logTextBox.AppendText(text);
            }
        }

        private void Initialize()
        {   // Open the config file and get the IP Address
            string setupFileName = "TachoInterface.ini";
            LogMessage("Checking for setup file: " + setupFileName + "...\r\n");
            if (File.Exists(setupFileName))
            {   // Found setup file, read the contents
                LogMessage("Found setup file, loading config items...\r\n");
                using (StreamReader setupFile = new StreamReader(setupFileName))
                {   // Store the IP setup data
                    StoreIpConfig(setupFile.ReadLine());
                }
            }
            else
            {
                MessageBox.Show("Configuration file could not be found, closing.","Configuration File Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                Environment.Exit(0);
            }
        }

        private void StoreIpConfig(string ipConfig)
        {
            string[] elements = ipConfig.Split(':');  // IPaddress:Port:Timeout
            // Store the IP Address if it is valid
            if (!IPAddress.TryParse(elements[0], out m_ipAddress))
            {   // Invalid ip address, use loopback
                LogMessage("Invalid IP Address: " + elements[0] + ", using: " + IPAddress.Loopback.ToString());
                m_ipAddress = IPAddress.Loopback;
            }
            // Store the port to be used
            m_port = Convert.ToInt32(elements[1]);
            // Store the timeout to use
            m_timeout = Convert.ToInt32(elements[2]);
        }

        private void StartListenerThread()
        {   // Create a new listener
            CreateNewListener();
            // Kick off the thread to handle connections and messages
            m_listenerThread = new Thread(new ThreadStart(ListenerThread));
            m_listenerThread.Start();
            LogMessage(DateTime.Now + ": Started listener thread\r\n");
        }

        private void CreateNewListener()
        {   // Setup the listener
            IPEndPoint endPoint = new IPEndPoint(m_ipAddress, m_port);
            m_listener = new TcpListener(endPoint);
            m_listener.Start();
            LogMessage("Started tcp/ip listener\r\n");
        }

        private void ReconnectSocket()
        {   // Close the existing connection and such
            m_listener.Stop();
            m_heartbeatThread.Abort();
            CreateNewListener();
            // Wait for the new connection
            WaitForSocketConnection();
        }

        private void WaitForSocketConnection()
        {   // Wait for the connection
            LogMessage(DateTime.Now + ": Waiting for socket...\r\n");
            m_socket = m_listener.AcceptSocket();
            LogMessage(DateTime.Now + ": Accepted connection from: " + m_socket.RemoteEndPoint.ToString() + "\r\n");
            // Create a new network stream for message handling
            Stream stream = new NetworkStream(m_socket);
            stream.ReadTimeout = m_timeout;
            m_reader = new StreamReader(stream);
            m_writer = new StreamWriter(stream);
            m_writer.AutoFlush = true;
            // Start a heartbeat thread
            m_heartbeatThread = new Thread(new ThreadStart(HeartbeatThread));
            m_heartbeatThread.Start();
        }

        public void ListenerThread()
        {   // Wait for the connection
            WaitForSocketConnection();
            // Listen forever
            while (true)
            {
                try
                {   // Check if socket must be reconnected
                    if (!m_socket.Connected)
                    {
                        ReconnectSocket();
                    }
                    try
                    {   // Wait for messages
                        string tachographData;
                        while (m_socket.Connected)
                        {
                                tachographData = m_reader.ReadLine();
                                if ((tachographData != "") && (tachographData != null))
                                {   // Message received, process it
                                    LogMessage(DateTime.Now + ": Rx - " + tachographData + "\r\n");
                                    string status = HandleMessage(tachographData);
                                    string[] message = tachographData.Split(';');
                                    string response = message[0] + ";" + status;
                                    if ((status != "") && (status != null))
                                    {
                                        m_writer.WriteLine(response);
                                        LogMessage(DateTime.Now + ": Sent response to CCRT - " + response + "\r\n");
                                    }
                                    else
                                    {
                                        LogMessage(DateTime.Now + ": No response required to CCRT\r\n");
                                    }
                                }
                                else
                                {   // No message received
                                    LogMessage(DateTime.Now + ": No message received from client!");
                                }
                        }
                    }
                    catch (Exception e)
                    {
                        LogMessage("ERROR: " + e.Message + "\r\n");
                    }
                }
                catch (SocketException excpt)
                {
                    LogMessage("ERROR waiting for socket: " + excpt.Message + "\r\n");
                }
            }
        }

        public void HeartbeatThread()
        {
            while (m_socket.Connected)
            {   // Send the heartbeat message
                string message = "STHeartbeat;";
                try
                {
                    // Write the message and wait for the response
                    m_writer.WriteLine(message);
                    LogMessage(DateTime.Now + ": Tx - " + message + "\r\n");
#if false
                    if (m_reader.Peek() > 0)
                    {
                        string response = m_reader.ReadLine();
                        if ((response != "") && (response != null))
                        {
                            LogMessage(DateTime.Now + ": Heartbeat response: " + response + "\r\n");
                        }
                        else
                        {
                            LogMessage(DateTime.Now + ": No heartbeat response!\r\n");
                        }
                    }
#endif 
                }
                catch (Exception excpt)
                {
                    LogMessage("ERROR: Heartbeat Thread - " + excpt.Message + "\r\n");
                }
                // Wait a bit before the next heartbeat
                Thread.Sleep(5000);
            }
        }

        private string HandleMessage(string message)
        {   // Break the message into its components
            string[] messageParts;
            messageParts = message.Split(';');  // MessageTag;VIN;CalConst
            string result = "Testing";
            // Determine the message type that was received
            if ("CalConstant" == messageParts[0])
            {   // Write the VIN to the tachograph
                int vinStatus = TachDllInterface.writeVin(messageParts[1]);
                LogMessage(DateTime.Now + ": Wrote vin (" + messageParts[1] + ") to module - " + vinStatus.ToString() + "\r\n");
                // Write the cal constant to the tachograph
                int kStatus = TachDllInterface.writeK(Convert.ToInt32(messageParts[2]));
                LogMessage(DateTime.Now + ": Wrote K (" + messageParts[2] + ") to module - " + kStatus.ToString() + "\r\n");
                // Write the cal date to the module
                DateTime currentdate = DateTime.Now;
                int dateStatus = TachDllInterface.writeDate(currentdate.Day, currentdate.Month, currentdate.Year);
                LogMessage(DateTime.Now + ": Wrote Date (day: " + currentdate.Day.ToString() + ", month: " +
                           currentdate.Month.ToString() + ", year: " + currentdate.Year.ToString() + " - " + 
                           dateStatus.ToString() + "\r\n");
                // Determine the result of the updates
                if ((vinStatus == SUCCESS) && (kStatus == SUCCESS) && (dateStatus == SUCCESS))
                {
                    result = "Pass";
                }
                else
                {
                    result = "Fail";
                }
                LogMessage("Tachograph update status: " + result + "\r\n");
            }
            else if ("STHeartbeat" == messageParts[0])
            {   // Handle the heartbeat message
                result = "";
            }
            else
            {   // Unknown message - do nothing with this
                result = "Fail";
                LogMessage("Received unsupported message: " + messageParts[0] + "\r\n");
            }
            return result;
        }


        private System.Windows.Forms.TextBox m_logTextBox;
        private IPAddress   m_ipAddress;
        private Socket      m_socket;
        private TcpListener m_listener;
        private Thread      m_listenerThread;
        private Thread      m_heartbeatThread;
        private int m_port;
        private int m_timeout;
        private const int SUCCESS = 0;
        private StreamReader m_reader;
        private StreamWriter m_writer;
    }
}
