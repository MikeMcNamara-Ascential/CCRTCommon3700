using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Windows.Forms;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Net.WebSockets;
using System.Runtime.Serialization.Json;
using System.Diagnostics;

namespace MES_Data_Interface
{
    public delegate void LogMessageDelegate(String message);

    public delegate void ListenerClosedEventHandler(Object sender, EventArgs e);

    class RequestHandler
    {
        String qnxIpAddress;
        String qnxListenerIpAddress;
        String qnxListenerTcpPort;
        String mesHostname;
        Boolean getDispositionBeforeUpdate;
        Boolean requestCloseListener;
        Boolean requestRestartListener;
        Boolean socketConnected;
        LogMessageDelegate LogMessage;
        public Boolean ListenerActive { get; private set; }
        public event ListenerClosedEventHandler ListenerClosed;
        Socket qnxSocket;
        BuildStringOptions buildStringOptions;

        String dumpFile = Path.Combine(Application.StartupPath, "sendfailures.lst");
        String retryFile = Path.Combine(Application.StartupPath, "sendretries.lst");
        Boolean checkForPendingRecords;

        public RequestHandler(Settings settings, LogMessageDelegate function)
        {
            LogMessage = new LogMessageDelegate(function);
            socketConnected = false;
            RestartListener(settings);
            ListenerClosed += ResultHandler_ListenerClosed;
        }

        void ResultHandler_ListenerClosed(object sender, EventArgs e)
        {
            LogMessage("ListenerClosed event raised!");
            if (requestRestartListener) RestartListener();
        }

        public void RestartListener(Settings newSettings)
        {
            qnxIpAddress = newSettings.QnxIpAddress;
            qnxListenerIpAddress = newSettings.QnxListenerIpAddress;
            qnxListenerTcpPort = newSettings.QnxListenerTcpPort;
            mesHostname = newSettings.MesHostName;
            getDispositionBeforeUpdate = newSettings.GetDispositionBeforeUpdate;

            if (!ListenerActive) RestartListener();
            else RestartListenerHelper();
        }

        public void RestartListener()
        {
            LogMessage("Starting Listener Thread...");
            SetListenerActive(true);
            requestRestartListener = false;
            Thread listenerThread = new Thread(new ThreadStart(RunListener));

            listenerThread.Start();
        }

        void RestartListenerHelper()
        {
            DialogResult dlgResult = DialogResult.None;
            Boolean forceClose = false;
            if (socketConnected)
            {
                dlgResult = MessageBox.Show("The connection listener needs to be restarted for settings to take effect, but a client is currently connected. Restart now?", "Attention",MessageBoxButtons.YesNo);
                forceClose = (dlgResult == DialogResult.Yes);
            }
            if (!socketConnected || (dlgResult == DialogResult.Yes))
            {
                requestRestartListener = true;
                CloseListener(forceClose);
            }
        }

        public void CloseListener(Boolean forceClose = false)
        {
            DialogResult dlgResult = DialogResult.None;
            if (socketConnected && !forceClose)
            {
                dlgResult = MessageBox.Show("MES Data Interface is still connected to a client. If you close now, the client may need to be restarted. Close anyway?", "Attention", MessageBoxButtons.YesNo);
            }
            if (!socketConnected || (dlgResult == DialogResult.Yes) || forceClose) requestCloseListener = true;
            else
            {
                requestCloseListener = false;
                requestRestartListener = false;
            }
        }

        void SetListenerActive(Boolean active)
        {
            requestCloseListener = false;
            ListenerActive = active;
        }

        void RunListener()
        {
            qnxSocket = null;
            Boolean keepConnection = true;
            System.Threading.Timer pendingTimer = new System.Threading.Timer(new TimerCallback(pendingTimer_Tick), null, 0, 60000);
            checkForPendingRecords = false;
            try
            {
                IPAddress listenerIpAddress;
                Int32 listenerTcpPort;

                try
                {
                    listenerIpAddress = IPAddress.Parse(qnxListenerIpAddress);
                }
                catch
                {
                    listenerIpAddress = IPAddress.Any;
                }
                try
                {
                    listenerTcpPort = Int32.Parse(qnxListenerTcpPort);
                }
                catch
                {
                    listenerTcpPort = 0;
                }
                LogMessage("Opening TcpListener on IP Address " + listenerIpAddress.ToString() + " Port " + listenerTcpPort.ToString());
                TcpListener listener = new TcpListener(listenerIpAddress, listenerTcpPort);

                listener.Start();
                LogMessage("TcpListener started...");

                Byte[] bytes = new Byte[4096];
                List<Byte> byteBuffer = new List<Byte>();
                Stopwatch watchdog = new Stopwatch();

                LogMessage("Listening for incoming client connections...");

                while (!requestCloseListener)
                {
                    if (listener.Pending())
                    {
                        LogMessage("Accepting client connection...");
                        Socket socket = listener.AcceptSocket();
                        socket.ReceiveTimeout = 500;
                        keepConnection = true;
                        socketConnected = true;

                        //using (NetworkStream stream = client.GetStream())
                        {
                            //LogMessage(String.Format("Using Stream {0}", stream.ToString()));
                            qnxSocket = socket;
                            while (keepConnection && !requestCloseListener)
                            {
                                if (checkForPendingRecords) SendPendingRecords();
                                try
                                {
                                    // Read available data into buffer
                                    Int32 i = 0;

                                    //watchdog.Reset();
                                    //watchdog.Start();
                                    try
                                    {
                                        //LogMessage(String.Format("Socket status is {0}", socket.Poll(100,SelectMode.SelectRead)));
                                        //LogMessage(String.Format("Data available - {0} bytes", socket.Available));
                                        if (socket.Available != 0) i = socket.Receive(bytes);
                                    }
                                    catch (SocketException err)
                                    {
                                        LogMessage(String.Format("Socket Exception occurred - {0}: {1}", err.SocketErrorCode.ToString(), err.Message));
                                    }
                                    catch (Exception err)
                                    {
                                        LogMessage(String.Format("Exception occurred - {0}", err));
                                        i = 0;
                                    }
                                    //i = bytes.Length;
                                    //i = stream.Read(bytes, 0, bytes.Length);
                                    //watchdog.Stop();

                                    if (i != 0)
                                    {
                                        for (int idx = 0; idx < i; idx++)
                                        {
                                            byteBuffer.Add(bytes[idx]);
                                        }
                                    }
                                    else
                                    {
                                        Boolean dataAvailable = false;
                                        Boolean attention = socket.Poll(250, SelectMode.SelectRead);
                                        if (attention) 
                                        {
                                            Thread.Sleep(500);
                                            dataAvailable = (socket.Available > 0);
                                        }
                                        keepConnection = (!attention) || (attention && dataAvailable);
                                        if (!keepConnection)
                                        {
                                            LogMessage("Connection was closed by remote client.");
                                        }
                                        else Thread.Sleep(500);
                                    }
                                    //stream.Write(Encoding.ASCII.GetBytes("Response"), 0, 8);
                                    //socket.Send(Encoding.ASCII.GetBytes("Response"));
                                    // Process complete messages from buffer
                                    ProcessMessageBuffer(ref byteBuffer);
                                }
                                catch (IOException err)
                                {
                                    if (err.InnerException != null)
                                    {
                                        if (typeof(SocketException).Equals(err.InnerException.GetType()))
                                        {
                                            if ((err.InnerException as SocketException).SocketErrorCode == SocketError.TimedOut)
                                            {
                                                keepConnection = true;
                                            }
                                            else
                                            {
                                                LogMessage(String.Format("Socket Exception occured while listening for incoming results: ({0}) - ", (err.InnerException as SocketException).SocketErrorCode, (err.InnerException as SocketException).Message));
                                                keepConnection = false;
                                            }
                                        }
                                    }
                                }
                                catch(Exception err)
                                {
                                    LogMessage(String.Format("Exception occured while listening for incoming results: {0}", err));
                                    keepConnection = false;
                                }
                            }
                            qnxSocket = null;
                        }
                        LogMessage("Closing client connection...");
                        //client.Close();
                        socket.Close();
                        LogMessage("Closing client connection... closed");
                        //run = false;
                        socketConnected = false;
                    }
                    else
                    {
                        Thread.Sleep(1000);
                        if (checkForPendingRecords) SendPendingRecords();
                    }
                }
                listener.Stop();
                pendingTimer.Change(0, 0);
                LogMessage("TcpListener stopped...");
            }
            catch
            {
            }
            finally
            {
                LogMessage("Ending Listener Thread...");
                SetListenerActive(false);
                ListenerClosed(this, EventArgs.Empty);
            }
        }

        void pendingTimer_Tick(object o)
        {
            checkForPendingRecords = true;
        }

        void ProcessMessageBuffer(ref List<Byte> byteBuffer)
        {
            Boolean done = false;
            while ((byteBuffer.Count > 0) && !done)
            {
                try
                {
                    //Int32 messageStart = data.IndexOf((char)0x02);
                    Int32 messageStart = byteBuffer.IndexOf((Byte)0x02);
                    //Int32 messageEnd = data.IndexOf((char)0x03);
                    Int32 messageEnd = byteBuffer.IndexOf((Byte)0x03);

                    // Account for if the checksum just so happens to be the same as our ETX
                    if ((messageEnd + 1) < byteBuffer.Count)
                    {
                        if (byteBuffer[messageEnd + 1] == (Byte)0x03) messageEnd += 1;
                    }

                    if (messageStart != -1)
                    {
                        if (messageEnd == -1)
                        {
                            // Wait for end of incoming message
                            done = true;
                        }
                        else if (messageEnd > messageStart)
                        {
                            // process complete message
                            //String message = data.Substring(messageStart, ((messageEnd - messageStart) + 1));
                            String message = ASCIIEncoding.ASCII.GetString( byteBuffer.GetRange(messageStart, (messageEnd - messageStart) + 1).ToArray());
                            Byte checksumByte = byteBuffer[messageEnd - 1];
                            ProcessIndividualMessage(message, checksumByte);
                            // remove processed message from buffer
                            //LogMessage(String.Format("Buffer Length = {0}, New Start Index = {1}", byteBuffer.Count, (messageEnd + 1)));
                            // data = data.Substring(messageEnd + 1);
                            //data = data.Remove(0, Math.Min(messageEnd + 1, data.Length));
                            byteBuffer.RemoveRange(0, messageEnd + 1);
                        }
                        else if (messageEnd < messageStart)
                        {
                            // trash fragment
                            // data = data.Substring(messageEnd + 1);
                            //data = data.Remove(0, Math.Min(messageEnd + 1, data.Length));
                            byteBuffer.RemoveRange(0, messageEnd + 1);
                        }
                    }
                    else
                    {
                        if (messageEnd != -1)
                        {
                            // trash fragment
                            // data = data.Substring(messageEnd + 1);
                            //data = data.Remove(0, Math.Min(messageEnd + 1, data.Length));
                            byteBuffer.RemoveRange(0, messageEnd + 1);
                        }
                        else
                        {
                            // wait for message to come in
                            done = true;
                        }
                    }
                }
                catch (Exception err)
                {
                    LogMessage(String.Format("Exception occurred when processing message buffer - {0}", err));
                    // wait for message to complete
                    done = true;
                }
            }
        }

        void ProcessIndividualMessage(String message, Byte checksumByte)
        {
            LogMessage(String.Format("Received a message ({0} bytes)", message.Length));
            LogMessage(String.Format("Incoming message - {0}", message));
            // Remove Start Byte and End Byte
            String messageString = message.Substring(1, message.Length - 2);
            // Validate checksum
            UInt16 incomingChecksum = Convert.ToUInt16(checksumByte);
            String checkedMessage = messageString.Substring(0, messageString.Length - 1);
            UInt16 ourChecksum = Convert.ToUInt16(Utils.CalculateChecksum(checkedMessage) & 0xFF);
            if (ourChecksum == incomingChecksum)
            {

                List<String> fields = new List<String>(checkedMessage.Split(new Char[] { '|' }, StringSplitOptions.RemoveEmptyEntries));
                if (fields.Count > 0)
                {
                    if (fields[0] == "UpdateDisposition")
                    {
                        if (fields.Count == 5)
                        {
                            String vehicleVin = fields[1];
                            String successStatus = fields[2];
                            String pitchName = fields[3];
                            String checkItemString = fields[4];
                            LogMessage("Processing result for VIN " + vehicleVin);

                            DispositionMessage dispositionMessage;
                            Boolean errorGettingDisposition = false;
                            // Get DispositionMessage to update
                            if (getDispositionBeforeUpdate) dispositionMessage = GetDispositionByVin(vehicleVin);
                            else dispositionMessage = new DispositionMessage();

                            if (getDispositionBeforeUpdate)
                            {
                                if (!String.IsNullOrEmpty(dispositionMessage.Success)) 
                                    errorGettingDisposition = (!dispositionMessage.Success.Equals("true", StringComparison.OrdinalIgnoreCase));
                            }

                            if (!errorGettingDisposition)
                            {
                                Boolean dispositionUpdated = false;

                                if (getDispositionBeforeUpdate)
                                {
                                    foreach (DispositionLine pitch in dispositionMessage.PitchList)
                                    {
                                        if (pitch.PitchName == pitchName)
                                        {
                                            //LogMessage(String.Format("Updating disposition for existing pitch with PitchName: {0}", pitchName));
                                            pitch.CheckItems = UpdateCheckItems(checkItemString);
                                            dispositionUpdated = true;
                                            break;
                                        }
                                    }
                                }

                                if (!dispositionUpdated)
                                {
                                    dispositionMessage.Vin = vehicleVin;
                                    DispositionLine newPitchItem = new DispositionLine();
                                    //LogMessage(String.Format("Updating disposition with new pitch with PitchName: {0}", pitchName));
                                    newPitchItem.PitchName = pitchName;
                                    newPitchItem.CheckItems = UpdateCheckItems(checkItemString);
                                    dispositionMessage.PitchList = new List<DispositionLine>();
                                    dispositionMessage.PitchList.Add(newPitchItem);
                                }

                                if (!UpdateDisposition(dispositionMessage))
                                {
                                    LogMessage(String.Format("Could not update disposition for VIN {0}", vehicleVin));
                                    ArchiveSendFailures(dispositionMessage);
                                }
                            }
                            else
                            {
                                LogMessage(String.Format("Error getting disposition to update for VIN {0}: {1}", vehicleVin, dispositionMessage.Error));
                            }
                        }
                        else
                        {
                            LogMessage("Invalid result message");
                        }
                    }
                    else if (fields[0] == "GetOptionCodesByVin")
                    {
                        if (fields.Count == 2)
                        {
                            try
                            {
                                String vehicleVin = fields[1];
                                Byte[] optionString = new Byte[]{};
                                OptionCodesMessage optionCodesMessage = GetOptionCodesByVin(vehicleVin);
                                if (optionCodesMessage.Success.Equals("False", StringComparison.OrdinalIgnoreCase))
                                {
                                    optionString = BuildNoInfoString();
                                }
                                else
                                {
                                    optionString = BuildOptionString(optionCodesMessage);
                                }
                                SendVehicleBuildResponse(optionString);
                            }
                            catch (Exception err)
                            {
                                LogMessage(String.Format("Error: Could not build option string from OptionCodesMessage - {0}", err));
                                SendNak();
                            }
                        }
                        else
                        {
                            LogMessage("Invalid build request");
                            SendNak();
                        }
                    }
                    else
                    {
                        LogMessage("Invalid request");
                        SendNak();
                    }
                }
            }
            else
            {
                LogMessage(String.Format("Not processing message: Invalid checksum (calculated: {0}, received: {1})", ourChecksum, incomingChecksum));
                if (messageString.Contains("GetOptionCodesByVin")) SendNak();
            }
        }

        private void SendNak()
        {
            SendVehicleBuildResponse(new Byte[] { (Byte) 0x15});
        }

        private void SendVehicleBuildResponse(Byte[] optionString)
        {
            if (qnxSocket == null)
            {
                LogMessage("Cannot send vehicle build response: Null socket reference");
            }
            else if (!qnxSocket.Connected)
            {
                LogMessage("Cannot send vehicle build response: Network socket is not connected");
            }
            else
            {
                LogMessage(String.Format("Sending vehicle build response - {0}\n checksum: 0x{1:X2}", Encoding.ASCII.GetString(optionString), optionString[optionString.Length-1]));
                qnxSocket.Send(optionString);
            }
        }

        private Byte[] BuildNoInfoString()
        {
            return (new Byte[] { (Byte)0x11 });
        }

        private Byte[] BuildOptionString(OptionCodesMessage optionCodesMessage)
        {
            LoadBuildStringOptions();
            String ackMessage = String.Format("{0}", Convert.ToChar(0x06));
            String optionString = String.Empty;
            String partNumberSubstring = BuildPartNumberSubstring(optionCodesMessage);
            String testCodeSubstring = BuildTestCodeSubstring(optionCodesMessage);
            String messageInfoSubstring = BuildMessageInfoSubstring(optionCodesMessage, partNumberSubstring.Length, testCodeSubstring.Length);
            String dataInterfaceInfoString = DateTime.Now.ToString("ddMMyyHHmm");
            UInt16 checksum = Utils.CalculateChecksum(messageInfoSubstring + partNumberSubstring + testCodeSubstring + dataInterfaceInfoString);
            optionString = ackMessage + messageInfoSubstring + partNumberSubstring + testCodeSubstring + dataInterfaceInfoString;
            List<Byte> optionStringBytes = new List<Byte>(Encoding.ASCII.GetBytes(optionString));
            optionStringBytes.Add((Byte)(checksum));
            return optionStringBytes.ToArray();
        }

        private String BuildMessageInfoSubstring(OptionCodesMessage optionCodesMessage, Int32 partNumberSectionLength, Int32 testCodeSectionLength)
        {
            String buildDate = DateTime.Now.ToString("ddMMyyyy");
            String buildTime = DateTime.Now.ToString("HHmm");
            String dummySeqNum = "0000";
            String returnString = String.Format("{0}Build0052{1:0000}{2:0000}{3}{4}{5}{6}P",
                new Object[] { Convert.ToChar((Byte)0x01), partNumberSectionLength, testCodeSectionLength, optionCodesMessage.Vin, buildDate, buildTime, dummySeqNum });
            return returnString;
        }

        private String BuildPartNumberSubstring(OptionCodesMessage optionCodesMessage)
        {
            //LogMessage("Extracting part numbers");
            String returnString = String.Empty;
            foreach (PNGroupCodePair pair in buildStringOptions.PartNumberList)
            {
                OptionCodeRow option = null;
                String partNumber = String.Empty;
                if (optionCodesMessage.OptionCodes != null)
                {
                    if (optionCodesMessage.OptionCodes.Exists(x => x.Group == pair.ItemGroupCode))
                        option = optionCodesMessage.OptionCodes.Find(x => x.Group == pair.ItemGroupCode);
                }
                if (option == null)
                    partNumber += new String('0', pair.PartNumberLength);
                else
                {
                    String format = String.Format("{{0:{0}}}",pair.PartNumberLength);
                    partNumber += String.Format(format, option.OptionCode);
                }
                //LogMessage(String.Format("\tPart Number {0} ({1}): {2}", pair.ItemName, pair.ItemGroupCode, partNumber));
                returnString += partNumber;
            }
            return returnString;
        }

        private String BuildTestCodeSubstring(OptionCodesMessage optionCodesMessage)
        {
            LogMessage("Extracting test codes");
            String returnString = String.Empty;
            foreach (GroupCodePair pair in buildStringOptions.TestCodeList)
            {
                OptionCodeRow option = null;
                String testCode = String.Empty;
                if (optionCodesMessage.OptionCodes != null)
                {
                    if (optionCodesMessage.OptionCodes.Exists(x => x.Group == pair.ItemGroupCode))
                        option = optionCodesMessage.OptionCodes.Find(x => x.Group == pair.ItemGroupCode);
                }
                if (option == null)
                    testCode += new String('0', 4);
                else
                {
                    testCode += String.Format("{0:4}", option.OptionCode);
                }
                LogMessage(String.Format("\tPart Number {0} ({1}): {2}", pair.ItemName, pair.ItemGroupCode, testCode));
                returnString += testCode;
            }
            return returnString;
        }

        private void LoadBuildStringOptions()
        {
            String buildStringOptionsFileName = Path.Combine(Application.StartupPath, "BuildStringOptions.xml");
            if (!File.Exists(buildStringOptionsFileName))
            {
                if (DialogResult.Yes == MessageBox.Show("BuildStringOptions.xml is missing. Create a new empty one?", "File missing", MessageBoxButtons.YesNo))
                {
                    buildStringOptions = new BuildStringOptions(true);
                    XmlSerializer serializer = new XmlSerializer(typeof(BuildStringOptions));
                    FileStream fs = new FileStream(buildStringOptionsFileName, FileMode.CreateNew);
                    XmlWriter writer = new XmlTextWriter(fs, Encoding.Default);
                    serializer.Serialize(writer, (buildStringOptions as BuildStringOptions));
                    writer.Close();
                }
                else
                {
                    throw new FileNotFoundException("Could not find file BuildStringOptions.xml");
                }
            }
            else
            {
                XmlSerializer serializer = new XmlSerializer(typeof(BuildStringOptions));
                FileStream fs = new FileStream(buildStringOptionsFileName, FileMode.Open);
                XmlReader reader = new XmlTextReader(fs);
                buildStringOptions = ((BuildStringOptions)serializer.Deserialize(reader));
                reader.Close();
            }
        }

        List<DispositionTest> UpdateCheckItems(String checkItemString)
        {
            List<String> items = new List<String>(checkItemString.Split(new Char[] { '@' }, StringSplitOptions.RemoveEmptyEntries));
            List<DispositionTest> checkItemList = new List<DispositionTest>();

            //LogMessage(String.Format("CheckItems: ({0} items)", items.Count));
            try
            {
                foreach (String item in items)
                {
                    DispositionTest dispositionTestItem = new DispositionTest();
                    List<String> itemFields = new List<String>(item.Split(new Char[] { '!' }, StringSplitOptions.RemoveEmptyEntries));
                    //LogMessage(String.Format("\t["));
                    foreach (String field in itemFields)
                    {
                        String[] fieldToValue = field.Split(new Char[] { ',' });
                        //LogMessage(String.Format("\t\t{0} : {1}", fieldToValue[0], fieldToValue[1]));
                        switch (fieldToValue[0])
                        {
                            case "Code":
                                dispositionTestItem.Code = fieldToValue[1];
                                break;
                            case "Message":
                                dispositionTestItem.Message = fieldToValue[1];
                                break;
                            case "Status":
                                dispositionTestItem.Status = Decimal.Parse(fieldToValue[1]);
                                break;
                            case "Value":
                                dispositionTestItem.Value = Decimal.Parse(fieldToValue[1]);
                                break;
                            case "Attribute":
                                dispositionTestItem.Attribute = fieldToValue[1];
                                break;
                            case "UOM":
                                dispositionTestItem.UOM = fieldToValue[1];
                                break;
                            case "MaxValue":
                                dispositionTestItem.MaxValue = Decimal.Parse(fieldToValue[1]);
                                break;
                            case "MinValue":
                                dispositionTestItem.MinValue = Decimal.Parse(fieldToValue[1]);
                                break;
                            case "StartTime":
                                dispositionTestItem.StartTime = DateTime.Parse(fieldToValue[1]);
                                break;
                            case "EndTime":
                                dispositionTestItem.EndTime = DateTime.Parse(fieldToValue[1]);
                                break;
                            case "TargetValue":
                                dispositionTestItem.TargetValue = Decimal.Parse(fieldToValue[1]);
                                break;
                            case "AttributeTarget":
                                dispositionTestItem.AttributeTarget = fieldToValue[1];
                                break;
                            default:
                                break;
                        }
                    }
                    //LogMessage("\t]\n");
                    checkItemList.Add(dispositionTestItem);
                }
            }
            catch (Exception err)
            {
                LogMessage(String.Format("Exception updating check item list - {0}", err));
            }
            return checkItemList;
        }

        DispositionMessage GetDispositionByVin(String vin)
        {
            String url = String.Format("http://{0}/Quality/Provider/GetDispositionByVin?vin={1}", mesHostname, vin);
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "GET";

            DispositionMessage disposition;

            try
            {
                using (HttpWebResponse webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode != HttpStatusCode.OK)
                    {
                        throw new Exception(String.Format("MES Data Interface: Error connecting to MES Host (HTTP {0}: {1})",
                            webResponse.StatusCode, webResponse.StatusDescription));
                    }

                    DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(DispositionMessage));
                    MemoryStream responseMemoryStream = new MemoryStream();
                    webResponse.GetResponseStream().CopyTo(responseMemoryStream);

                    responseMemoryStream.Position = 0;
                    Object responseObj = serializer.ReadObject(responseMemoryStream);
                    disposition = responseObj as DispositionMessage;

                    responseMemoryStream.Position = 0;
                    using (StreamReader reader = new StreamReader(responseMemoryStream))
                    {
                        LogMessage(String.Format("MES Data Interface: Response received from host - {0}", reader.ReadToEnd()));
                    }
                }
            }
            catch (Exception err)
            {
                LogMessage(String.Format("MES Data Interface: Exception in GetDispositionByVin for URL ({0}), VIN ({1}): {2}",
                    url, vin, err));
                disposition = new DispositionMessage();
            }

            return disposition;
        }

        OptionCodesMessage GetOptionCodesByVin(String vin)
        {
            String url = String.Format("http://{0}/GetOptionCodesByVin?vin={1}", mesHostname, vin);
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "GET";
            webRequest.ContentType = "application/json";
            webRequest.Accept = "application/json";

            OptionCodesMessage optionCodes;

            try
            {
                using (HttpWebResponse webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode != HttpStatusCode.OK)
                    {
                        throw new Exception(String.Format("MES Data Interface: Error connecting to MES Host (HTTP {0}: {1})",
                            webResponse.StatusCode, webResponse.StatusDescription));
                    }

                    DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(OptionCodesMessage));
                    MemoryStream responseMemoryStream = new MemoryStream();
                    webResponse.GetResponseStream().CopyTo(responseMemoryStream);

                    responseMemoryStream.Position = 0;
                    Object responseObj = serializer.ReadObject(responseMemoryStream);
                    optionCodes = responseObj as OptionCodesMessage;

                    responseMemoryStream.Position = 0;
                    using (StreamReader reader = new StreamReader(responseMemoryStream))
                    {
                        LogMessage(String.Format("MES Data Interface: Response received from host - {0}", reader.ReadToEnd()));
                    }
                }
            }
            catch (Exception err)
            {
                LogMessage(String.Format("MES Data Interface: Exception in GetOptionCodesByVin for URL ({0}), VIN ({1}): {2}",
                    url, vin, err));
                optionCodes = new OptionCodesMessage();
            }

            return optionCodes;
        }

        bool UpdateDisposition(DispositionMessage disposition, Boolean autoRetry = true)
        {
            String url = String.Format("http://{0}/UpdateDisposition", mesHostname);
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "POST";
            webRequest.ContentType = "application/json";
            webRequest.Accept = "application/json";

            Boolean success = true;

            try
            {
                DataContractJsonSerializer requestSerializer = new DataContractJsonSerializer(typeof(DispositionMessage));
                MemoryStream requestMemoryStream = new MemoryStream();
                requestSerializer.WriteObject(requestMemoryStream, disposition);

                requestMemoryStream.Position = 0;
                Byte[] byteArray = requestMemoryStream.ToArray();
                LogMessage(Encoding.ASCII.GetString(byteArray));
                webRequest.ContentLength = byteArray.Length;

                // using (StreamWriter writer = new StreamWriter(webRequest.GetRequestStream()))
                //{
                using (Stream postStream = webRequest.GetRequestStream())
                {
                    postStream.Write(byteArray, 0, byteArray.Length);
                    postStream.Flush();
                }
                // }

                using (HttpWebResponse webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode != HttpStatusCode.OK)
                    {
                        LogMessage(String.Format("MES Data Interface: Server error (HTTP {0}: {1}).",
                            webResponse.StatusCode, webResponse.StatusDescription));
                        success = false;
                    }

                    DataContractJsonSerializer responseSerializer = new DataContractJsonSerializer(typeof(DispositionMessage));
                    MemoryStream responseMemoryStream = new MemoryStream();
                    webResponse.GetResponseStream().CopyTo(responseMemoryStream);

                    responseMemoryStream.Position = 0;
                    StreamReader reader = new StreamReader(responseMemoryStream);
                    LogMessage(String.Format("MES Data Interface: Response received from host for last sent result - {0}", reader.ReadToEnd()));

                    responseMemoryStream.Position = 0;
                    Object responseObj = responseSerializer.ReadObject(responseMemoryStream);
                    DispositionMessage dispositionResponse = responseObj as DispositionMessage;

                    if (dispositionResponse.Success == null || dispositionResponse.Success.Length == 0)
                    {
                        LogMessage(String.Format("Invalid response from host for last sent result for VIN {0} - Success field is null or empty!", disposition.Vin));
                        success = false;
                    }
                    else if (dispositionResponse.Success.Equals("True", StringComparison.OrdinalIgnoreCase))
                    {
                        LogMessage(String.Format("Successfully sent result to host for VIN {0}", disposition.Vin));
                        success = true;
                    }
                    else
                    {
                        LogMessage(String.Format("Error received from host for last sent result for VIN {0} - {1}", disposition.Vin, dispositionResponse.Error));
                        success = false;
                    }
                }
            }
            catch (WebException err)
            {
                LogMessage(String.Format("MES Data Interface: WebException in UpdateDisposition for URL ({0}): {1}",
                    url, err));
                if (autoRetry)
                {
                    LogMessage(String.Format("Saving DispositionMessage to be retried later"));
                    ScheduleSendRetry(disposition);
                    success = true;
                }
                else throw err;
            }
            catch (Exception err)
            {
                LogMessage(String.Format("MES Data Interface: Exception in UpdateDisposition for URL ({0}): {1}",
                    url, err));
                success = false;
            }

            return success;
        }

        private void ArchiveSendFailures(DispositionMessage dispositionMessage)
        {
            StringBuilder record = new StringBuilder();
            record.Append(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
            record.Append(" - \n");

            DataContractJsonSerializer requestSerializer = new DataContractJsonSerializer(typeof(DispositionMessage));
            MemoryStream requestMemoryStream = new MemoryStream();
            requestSerializer.WriteObject(requestMemoryStream, dispositionMessage);

            requestMemoryStream.Position = 0;
            Byte[] byteArray = requestMemoryStream.ToArray();
            record.Append(Encoding.UTF8.GetString(byteArray));
            record.Append("\n\n");

            FileStream fs = new FileStream(dumpFile, FileMode.Append);
            StreamWriter writer = new StreamWriter(fs);
            writer.Write(record.ToString());
            writer.Close();
        }

        private void ScheduleSendRetry(DispositionMessage dispositionMessage)
        {
            List<DispositionMessage> retryList;
            if (File.Exists(retryFile))
            {
                try
                {
                    XmlSerializer deserializer = new XmlSerializer(typeof(List<DispositionMessage>));
                    FileStream ifs = new FileStream(retryFile, FileMode.Open);
                    XmlReader reader = new XmlTextReader(ifs);
                    retryList = ((List<DispositionMessage>)deserializer.Deserialize(reader));
                    ifs.Close();
                }
                catch
                {
                    retryList = new List<DispositionMessage>();
                }
            }
            else
            {
                retryList = new List<DispositionMessage>();
            }

            retryList.Add(dispositionMessage);

            XmlSerializer serializer = new XmlSerializer(typeof(List<DispositionMessage>));
            FileStream ofs = new FileStream(retryFile, FileMode.Create);
            serializer.Serialize(ofs, retryList);
            ofs.Flush();
            ofs.Close();
        }

        private void SendPendingRecords()
        {
            List<DispositionMessage> retryList = null;
            if (File.Exists(retryFile))
            {
                try
                {
                    XmlSerializer deserializer = new XmlSerializer(typeof(List<DispositionMessage>));
                    FileStream ifs = new FileStream(retryFile, FileMode.Open);
                    XmlReader reader = new XmlTextReader(ifs);
                    retryList = ((List<DispositionMessage>)deserializer.Deserialize(reader));
                    ifs.Close();
                }
                catch
                {
                    retryList = new List<DispositionMessage>();
                }
            }

            if (retryList != null)
            {
                while (retryList.Count > 0)
                {
                    try
                    {
                        Boolean success = UpdateDisposition(retryList[0], false);
                        if (!success)
                        {
                            ArchiveSendFailures(retryList[0]);
                        }
                        retryList.RemoveAt(0);
                    }
                    catch
                    {
                        break;
                    }
                }

                XmlSerializer serializer = new XmlSerializer(typeof(List<DispositionMessage>));
                FileStream ofs = new FileStream(retryFile, FileMode.Create);
                serializer.Serialize(ofs, retryList);
                ofs.Flush();
                ofs.Close();
            }

            checkForPendingRecords = false;
        }
    }

    public class Utils
    {
        public static UInt16 CalculateChecksum(String data)
        {
            UInt16 checksum = 0;
            for (UInt16 ii = 0; ii < data.Length; ii++)
            {
                checksum += (UInt16)data[ii];
            }
            return Convert.ToUInt16(checksum & 0xFFFF);
        }
    }
}
