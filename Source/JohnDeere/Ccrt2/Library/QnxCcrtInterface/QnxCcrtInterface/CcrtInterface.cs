using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Logger;

namespace QnxCcrtInterface
{
    public class CcrtInterface
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Create a new QNX CCRT Interface object.
        /// This object will connect to the QNX CCRT system to allow information to be exchanged between
        /// the two systems.
        /// </summary>
        /// <param name="ipAddress">IP Address for the QNX CCRT System.</param>
        /// <param name="port">Port to use to monitor for connection requests.</param>
        /// <param name="logger">Logger object to log information and events.</param>
        public CcrtInterface(IPAddress ipAddress, Int32 port, CcrtLogger logger)
        {   // Store the IP Address and port to use
            WriteLog("Creating CcrtInterface class");
            Port = port;
            SetQnxConnected = false;
            // Store the logger object to use
            LogFile = logger;
            // Create a new TCP/IP client
//            Thread connectThread = new Thread(new ThreadStart(ConnectTCP));
//            connectThread.Start();
//            ConnectTCP();
            m_printIOException = false;
            QnxIpAddress = ipAddress;
        }

        public void ConnectTCP()
        {
#if false
            StartListenerThread();
#else
            QnxComm = new TcpClient();
            try
            {
                QnxComm.Connect(new IPEndPoint(QnxIpAddress, Port));
                if (QnxComm.Connected)
                {
                    //                        WriteLog("Connected to " + QnxIpAddress.ToString() + ":" + Port.ToString());
                    Stream stream = QnxComm.GetStream();
                    CcrtCommReader = new StreamReader(stream);
                    CcrtCommReader.BaseStream.ReadTimeout = 5000;
                    CcrtCommWriter = new StreamWriter(stream);
                    CcrtCommWriter.AutoFlush = true;
                    SetQnxConnected = true;
                }
                else
                {
                    //                        WriteLog("Could not connect to " + QnxIpAddress.ToString() + ":" + Port.ToString());
                }
            }
            catch (SocketException excpt)
            {
                WriteLog("Socket Exception trying to connect to QNX - " + excpt.Message);
                SetQnxConnected = false;
            }
#endif
        }

        /// <summary>
        /// Change the IP Address that is being monitored.
        /// The 
        /// </summary>
        /// <param name="ipAddress"></param>
        public void ChangeIpAddress(IPAddress ipAddress)
        {   // Save the IP Address
            QnxIpAddress = ipAddress;
#if false
            if (ListenerThread == null)
            {
                StartListenerThread();
            }
            else
            {
                if (QnxListenerSocket != null)
                {
                    QnxListenerSocket.Disconnect(true);
                }
            }
#else
            if(QnxComm.Connected)
            {
                QnxComm.Close();
            }
            ConnectTCP();
#endif
        }

        /// <summary>
        /// Create a new listener to listen for connection requests.
        /// </summary>
        /// <returns>Flag indicating if the listener was created successfully.</returns>
        private Boolean CreateNewListener()
        {
            Boolean listenerStarted = false;
            IPEndPoint endPoint = new IPEndPoint(QnxIpAddress, Port);
            try
            {
                QnxListener = new TcpListener(endPoint);
                LogFile.Log("Created new TcpListener on IPEndPoint: " + endPoint.ToString(), ProcessName);
                QnxListener.Start();
                LogFile.Log("Started TCP/IP Listener on " + QnxIpAddress.ToString() + ":" + Port.ToString(), ProcessName);
                listenerStarted = true;
            }
            catch (SocketException excpt)
            {
                LogFile.Log("Could not create TCP/IP Listener for " + QnxIpAddress.ToString() +
                            ":" + Port.ToString() + " - " + excpt.Message, ProcessName);
            }
            return listenerStarted;
        }

        /// <summary>
        /// Thread to Listen for connections and handle any incoming messages.
        /// </summary>
        public void ListenerThreadProcess()
        {   // Wait for the connection
            WaitForSocketConnection();
            // Listen forever
            while (true)
            {
                try
                {   // Check if the socket must be reconnected
                    if (!QnxListenerSocket.Connected)
                    {
                        LogFile.Log("QnxListenerSocket no longer connected. Attempting to reconnect");
                        ReconnectSocket();
                    }
                    try
                    {
//                        String message;
                        while (QnxConnected)
                        {   // Read a message from the client
#if false
                            message = CcrtCommReader.ReadLine();
                            if ((message != null) && (message.Length > 0))
                            {   // Received a message, process it
                                LogFile.Log("RX: " + message, ProcessName);
                            }
#else   
                            if (Monitor.TryEnter(QnxListenerSocket))
                            {
                                bool origBlockingState = QnxListenerSocket.Blocking;
                                int origTimeout = QnxListenerSocket.SendTimeout;
                                int rx;
                                try
                                {
                                    byte[] tmp = new byte[1];
                                    QnxListenerSocket.SendTimeout = 10;
                                    rx = QnxListenerSocket.Send(tmp);
                                }
                                catch (SocketException e)
                                {
                                    // 10035 == WSAEWOULDBLOCK
                                    if (e.NativeErrorCode.Equals(10035))
                                    {
                                        LogFile.Log("Still Connected, but the Send would block");
                                    }
                                    else
                                    {
                                        SetQnxConnected = false;
                                        LogFile.Log("Disconnected: error code: " + e.NativeErrorCode);
                                    }
                                }
                                finally
                                {
                                    QnxListenerSocket.Blocking = origBlockingState;
                                    QnxListenerSocket.SendTimeout = origTimeout;
                                    bool connected = QnxListenerSocket.Connected;
                                    Monitor.Exit(QnxListenerSocket);
                                    if (!connected)
                                        ReconnectSocket();
                                }
                            }
                            Thread.Sleep(500);
#endif
                        }
                        if (QnxListenerSocket != null)
                        {
                            QnxListenerSocket.Shutdown(SocketShutdown.Both);
                            QnxListenerSocket.Close();

                            //Shutdown();
                        }
                    }
                    catch (Exception excpt)
                    {
                        LogFile.Log("EXCEPTION: " + excpt.Message, ProcessName);
                        // Wait a bit for things to settle down
                        Thread.Sleep(5000);
                    }
                }
                catch (SocketException excpt)
                {
                    LogFile.Log("ERROR waiting for socket: " + excpt.Message, ProcessName);
                }
                catch (NullReferenceException)
                {   // Nothing special to do.  Socket may have been deleted during settings change
                }
                catch (Exception e)
                {
                    LogFile.Log("Listner Thread error: " + e.ToString());
                }
                LogFile.Log("Unexpected exit from listener thread");
            }
        }

        /// <summary>
        /// Reconnect the socket. 
        /// The existing connection will first be closed. Next, the function will wait for a new connection.
        /// </summary>
        public void ReconnectSocket()
        {   // Close the existing connection
            WriteLog("ReconnectSocket()");
            SetQnxConnected = false;
            QnxListener.Stop();
            if (CreateNewListener())
            {   // Wait for the new connection
                WaitForSocketConnection();
            }
        }

        /// <summary>
        /// Shutdown the listener and close the socket as the application is exiting
        /// </summary>
        public void Shutdown()
        {   // Shutdown the listener thread
            SetQnxConnected = false;
#if false
            try
            {// Consider not using Thread.Abort (alternative: flag)
                ListenerThread.Abort();
            }
            catch (NullReferenceException)
            {   // Nothing special to do since the thread may have already been stopped
            }
            // Close the connection
            QnxListener.Stop();
#else
            if (QnxComm.Connected)
            {
                QnxComm.Close();
            }
#endif
        }

        /// <summary>
        /// Start a thread to listen for connection requests.
        /// </summary>
        private Boolean StartListenerThread()
        {   // Create a new listener
            Boolean listenerStarted = CreateNewListener();
            if (listenerStarted)
            {   // Kick off the thread to handle connections and messages
                ListenerThread = new Thread(new ThreadStart(ListenerThreadProcess));
                ListenerThread.Start();
                LogFile.Log("Started listener thread", ProcessName);
            }
            return listenerStarted;
        }

        /// <summary>
        /// Wait for a message from the QNX CCRT system.
        /// After a message is received, pass the message back to the calling function.
        /// </summary>
        /// <returns>Message received from the QNX CCRT system.</returns>
        public String WaitForQnxCcrtMessage()
        {
            String rxMessage = "";
            Int32 bytesRead = 0;
            char[] buffer = new char[4096];
            Int32 orgReadTimeout = CcrtCommReader.BaseStream.ReadTimeout;
            // created Made timeout infinate to create a blocking read
            CcrtCommReader.BaseStream.ReadTimeout = Timeout.Infinite;
            // Keep looping for a message
            while (QnxConnected && String.IsNullOrEmpty(rxMessage))
            {
                if (Monitor.TryEnter(QnxListenerSocket))
                {
                    try
                    {
                        bytesRead = CcrtCommReader.Read(buffer, 0, 4096);
                    }
                    catch (IOException e)
                    {   // Nothing special to do.
                        if (!m_printIOException)
                        {
                            WriteLog("CcrtCommReader.Read - " + e.ToString());
                            m_printIOException = true;
                        }

                    }
                    finally
                    {
                        Monitor.Exit(QnxListenerSocket);
                    }
                }

                if (bytesRead > 0)
                {
                    rxMessage = new String(buffer);
                }

                Thread.Sleep(100);
                
                if (rxMessage.Length > 1)
                {
                    LogFile.Log("Received message from QNX CCRT: " + rxMessage, ProcessName);
                }
            }
            
            // Return to the default read timeout.
            //CcrtCommReader.BaseStream.ReadTimeout = orgReadTimeout;
            CcrtCommReader.BaseStream.ReadTimeout = Timeout.Infinite;
            return rxMessage;
        }

        /// <summary>
        /// Wait for a client to connect to tthis socket.
        /// </summary>
        private void WaitForSocketConnection()
        {   // Wait for the connection
            try
            {
                LogFile.Log("Waiting for socket...", ProcessName);
                //QnxListenerSocket = QnxListener.AcceptSocket();
                // Method Alt ++++++++++++++++++++++++++++++++++++++++++++++++++
                try
                {
                    ManualResetEvent clientConnected = new ManualResetEvent(false);

                    QnxListener.Start();
                    clientConnected.Reset();
                    var iasyncResult = QnxListener.BeginAcceptSocket((ar) =>
                    {
                        try
                        {
                            QnxListenerSocket = QnxListener.EndAcceptSocket(ar);
                            clientConnected.Set();
                        }
                        catch { LogFile.Log("Connection interrupted.");  } //LogFile.WriteLogMessagesToFile(); }
                    }, null);
                    clientConnected.WaitOne();
                }
                catch (ThreadAbortException ex)
                {
                    QnxListener.Stop();
                    // Depending on timing, this log statement may not make it to the file!
                    LogFile.Log("Abort requested. Stopped TcpListener on IPEndPoint: " + QnxListener.LocalEndpoint.ToString(), ProcessName);
                  
                    //Thread.Sleep(1500);
                    // Forcing the Write may cause a failure to abort the thread
                    // LogFile.WriteLogMessagesToFile();
                }
                // End Method Alt +++++++++++++++++++++++++++++++++++++++++++++++++

                LogFile.Log("Accepted connection from: " + QnxListenerSocket.RemoteEndPoint.ToString(), ProcessName);
                // Create a new network stream for message handling
                //IPAddress hostIp = IPAddress.Parse(QnxListenerSocket.RemoteEndPoint.ToString());
                //IPHostEntry Qhost = Dns.GetHostEntry(hostIp);
                //TcpClient nTcp = new TcpClient(Qhost.HostName, 23);
                Stream stream = new NetworkStream(QnxListenerSocket);
                CcrtCommReader = new StreamReader(stream);
                CcrtCommReader.BaseStream.ReadTimeout = 5000;
                CcrtCommWriter = new StreamWriter(stream);
                CcrtCommWriter.AutoFlush = true;
                SetQnxConnected = true;
            }
            catch (SocketException)
            {   // Nothing special to do.  Accept socket may have been interrupted by settings change
            }
        }

        /// <summary>
        /// Read data from the QNX CCRT system.
        /// </summary>
        /// <param name="dataTag">System data tag to read from the QNX CCRT system.</param>
        /// <returns>Value of the system data tag read from the QNX CCRT system.</returns>
        public String Read(String dataTag, bool cancel = false)
        {
            String result = "Update Interrupted";
            if (!cancel)
            {
                result = "Not Connected to QNX CCRT system";
            Boolean accessed = false;
                Int32 attempts = 1;
                Int32 msgAttempts = 3;
                if (QnxComm != null)
                {
                    if (!QnxComm.Connected)
                    {
                        ConnectTCP();
                    }
                    if (QnxComm.Connected)
                {   // Send the tag to the QNX CCRT system and then read the result
                        String message = "Read:" + dataTag + ";"; //Seperating char is ',' (no end char has been designated in WinCCRT server)
                    Int32 bytesRead = 0;
                    char[] rdMsg = new char[1024];
                    do
                    {
                            if (Monitor.TryEnter(QnxComm, 100))
                            {
                            accessed = true;
                            CcrtCommWriter.Write(message);
                                CcrtCommReader.BaseStream.ReadTimeout = 1000;
                            // Wait for the response
                            do
                            {       
                               try
                                {
                                    bytesRead = CcrtCommReader.Read(rdMsg, bytesRead, 1024);
                                        if (bytesRead > 0)
                                        {
                                            result = new String(rdMsg, 0, bytesRead);
                                            foreach (String rspMsg in result.Split(';'))
                                            {
                                                if (!String.IsNullOrEmpty(rspMsg))
                                                {
                                                    String[] resultParts = rspMsg.Split('=');
                                                    if (resultParts[0] != dataTag)
                                                    {
                                                        LogFile.Log("Error: Received data for " + resultParts[0] +
                                                                    " when reading dataTag: " + dataTag + " - rx msg: " + result);
                                                    }
                                                    else
                                                    {
                                                        result = resultParts[1];
                                                    }
                                                }
                                            }
                                            //LogFile.Log(message + result);
                                        }
                                    }
                                catch (IOException)
                                {   // Most likely a timeout trying to read data.
                                }
                            } while ((bytesRead != 0) && (attempts-- > 0) && rdMsg[bytesRead] != '\0');
                                Monitor.Exit(QnxComm);
                            }
                        else
                        {
                           LogFile.Log("Timeout Attempting to aquire lock in CcrtInterface::Read");
                        }
                        } while (!accessed && (msgAttempts-- > 0));
                    }
                }
            }
            return result;
        }

        /// <summary>
        /// Write data to the QNX CCRT system.
        /// </summary>
        /// <param name="dataTag">System data tag to write.</param>
        /// <param name="dataValue">Value of the system data tag.</param>
        /// <returns>Status of writing the data to the QNX CCRT system.</returns>
        public Int32 Write(String dataTag, String dataValue)
        {
            Int32 status = -1;
            Int32 sendAttempts = 3;
            if (QnxComm != null)
            {
                if (!QnxComm.Connected)
                {
                    ConnectTCP();
                }
                if (QnxComm.Connected)
                {
                    String message = "Write:" + dataTag + "," + dataValue + ";";// Seperating char is ',' (no end char has been designated in WinCCRT server)
                    do
                    {
                        if (Monitor.TryEnter(QnxComm))
                        {
                            //LogFile.Log("Sending message to QNX CCRT: [" + message + "]", ProcessName);
                            try
                            {
                                CcrtCommWriter.Write(message);
                                LogFile.Log("Sent message to QNX CCRT: [" + message + "]", ProcessName);
                                status = 0;
                            }
                            catch (Exception excpt)
                            {
                                LogFile.Log("ERROR: Could not send message to QNX CCRT - " + excpt.Message, ProcessName);
                                status = -1;
                            }
                            finally
                            {
                                Monitor.Exit(QnxComm);
                            }
                        }
                        else
                        {
                            Thread.Sleep(100);
                        }
                    } while ((status == -1) && (sendAttempts-- > 0));
                }
                //            LogFile.Log("Wrote " + dataTag + "," + dataValue + " to QNX CCRT - " + ((status == 0) ? "Success" : "Fail"));
            }
            return status;
        }

        /// <summary>
        /// Write data to a specific server in the QNX CCRT system.
        /// </summary>
        /// <param name="serverName">Name of the server to send the data to.</param>
        /// <param name="dataTag">System data tag to write.</param>
        /// <param name="dataValue">Value of the system data tag.</param>
        /// <returns>Status of writing the data to the QNX CCRT system.</returns>
        public Int32 ServerWrite(String serverName, String dataTag, String dataValue)
        {
            Int32 status = -1;
            Int32 sendAttempts = 3;
            if (QnxComm != null)
            {
                if (QnxComm.Connected)
                {
                    String message = "ServerWrite:" + serverName + "," + dataTag + "," + dataValue + ";";
                    do
                    {
                        if (Monitor.TryEnter(QnxComm))
                        {
                            //LogFile.Log("Sending message to QNX CCRT server: [" + message + "]", ProcessName);
                            try
                            {
                                CcrtCommWriter.Write(message);
                                LogFile.Log("Sent message to QNX CCRT server: [" + message + "]", ProcessName);
                                status = 0;
                            }
                            catch (Exception excpt)
                            {
                                LogFile.Log("ERROR: Could not send message to QNX CCRT server - " + excpt.Message, ProcessName);
                                status = -1;
                            }
                            finally
                            {
                                Monitor.Exit(QnxComm);
                            }
                        }
                        else
                        {
                            Thread.Sleep(100);
                        }
                    } while ((status == -1) && (sendAttempts-- > 0));
                }
                //            LogFile.Log("Wrote " + dataTag + "," + dataValue + " to QNX CCRT - " + ((status == 0) ? "Success" : "Fail"));
            }
            return status;
        }

        private void WriteLog(string msg)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"debug.log", true))
            {
                file.WriteLine(DateTime.Now + "\t" + msg);
                file.Close();
            }
        }

        // ----------------------------------------------------------------------------------------
        // Class Variables and Properties
        // ----------------------------------------------------------------------------------------

        private bool m_printIOException {get; set;}
        /// <summary>
        /// Get/Set the CCRT logger object.
        /// </summary>
        private CcrtLogger LogFile { get; set; }

        private IPAddress m_qnxIpAddress;
        /// <summary>
        /// Get/Set the IP Address for the QNX server.
        /// </summary>
        public IPAddress QnxIpAddress 
        {
            get
            {
                return m_qnxIpAddress;
            }
            set
            {
                m_qnxIpAddress = value;
                ConnectTCP();
            }
        }

        /// <summary>
        /// Get/Set the port to use for listening for connection requests.
        /// </summary>
        private Int32 Port { get; set; }

        /// <summary>
        /// Socket the is connected to the QNX system.
        /// </summary>
        private Socket QnxListenerSocket { get; set; }

        private TcpClient QnxComm { get; set; }

        /// <summary>
        /// Listener to use for listening for connection requests.
        /// </summary>
        private TcpListener QnxListener { get; set; }

        /// <summary>
        /// Thread to use for listening for connection requests.
        /// </summary>
        private Thread ListenerThread { get; set; }

        /// <summary>
        /// Heartbeat thread for ensuring clients are still connected.
        /// </summary>
        private Thread HeartBeatThread { get; set; }

        /// <summary>
        /// Get/Set the reader for reading messages from the QNX CCRT system.
        /// </summary>
        private StreamReader CcrtCommReader { get; set; }

        /// <summary>
        /// Get/Set the writer for writing message to the QNX CCRT system.
        /// </summary>
        private StreamWriter CcrtCommWriter { get; set; }

        /// <summary>
        /// Get the name of the class.
        /// </summary>
        private String ProcessName { get { return "CcrtInterface"; } }

        /// <summary>
        /// Flag to use for tracking the QNX connected status.
        /// </summary>
        private Boolean m_isQnxConnected;

        /// <summary>
        /// Set the flag indicating if QNX has connected.
        /// </summary>
        private Boolean SetQnxConnected { set { m_isQnxConnected = value; } }

        /// <summary>
        /// Get the flag indicating if the QNX system has connected.
        /// </summary>
        public Boolean QnxConnected { get { return m_isQnxConnected; } }
    }
}
