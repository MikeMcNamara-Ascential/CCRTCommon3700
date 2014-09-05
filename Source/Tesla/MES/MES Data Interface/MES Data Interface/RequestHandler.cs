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
        Boolean requestCloseListener;
        Boolean requestRestartListener;
        LogMessageDelegate LogMessage;
        public Boolean ListenerActive { get; private set; }
        public event ListenerClosedEventHandler ListenerClosed;
        NetworkStream qnxStream;

        public RequestHandler(Settings settings, LogMessageDelegate function)
        {
            LogMessage = new LogMessageDelegate(function);
            RestartListener(settings);
            ListenerClosed += ResultHandler_ListenerClosed;
        }

        void ResultHandler_ListenerClosed(object sender, EventArgs e)
        {
            if (requestRestartListener) RestartListener();
        }

        public void RestartListener(Settings newSettings)
        {
            qnxIpAddress = newSettings.QnxIpAddress;
            qnxListenerIpAddress = newSettings.QnxListenerIpAddress;
            qnxListenerTcpPort = newSettings.QnxListenerTcpPort;
            mesHostname = newSettings.MesHostName;

            if (!ListenerActive) RestartListener();
            else RestartListenerHelper();
        }

        public void RestartListener()
        {
            LogMessage("Starting Listener Thread...");
            SetListenerActive(true);
            Thread listenerThread = new Thread(new ThreadStart(RunListener));

            listenerThread.Start();
        }

        void RestartListenerHelper()
        {
            requestRestartListener = true;
            CloseListener();
        }

        public void CloseListener()
        {
            requestCloseListener = true;
        }

        void SetListenerActive(Boolean active)
        {
            requestCloseListener = false;
            requestRestartListener = false;
            ListenerActive = active;
        }

        void RunListener()
        {
            Boolean keepConnection = true;
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
                String data = String.Empty;
                Stopwatch watchdog = new Stopwatch();

                LogMessage("Listening for incoming client connections...");

                while (!requestCloseListener)
                {
                    if (listener.Pending())
                    {
                        LogMessage("Accepting client connection...");
                        TcpClient client = listener.AcceptTcpClient();
                        keepConnection = true;

                        using (NetworkStream stream = client.GetStream())
                        {
                            qnxStream = stream;
                            stream.ReadTimeout = 500;
                            while (keepConnection && !requestCloseListener)
                            {
                                try
                                {
                                    // Read available data into buffer
                                    Int32 i = 0;

                                    watchdog.Reset();
                                    watchdog.Start();
                                    i = stream.Read(bytes, 0, bytes.Length);
                                    watchdog.Stop();

                                    if (i != 0)
                                    {
                                        data += Encoding.ASCII.GetString(bytes, 0, i);
                                    }
                                    else
                                    {
                                        Thread.Sleep(500);
                                        keepConnection = (client.Connected && (watchdog.ElapsedMilliseconds > (long)50));
                                        if (!keepConnection) LogMessage("Connection was closed by remote client.");
                                        data = String.Empty;
                                    }
                                    // Process complete messages from buffer
                                    ProcessMessageBuffer(ref data);
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
                                    LogMessage(String.Format("Exception occured while listening for incoming results: {0}", err.Message));
                                    keepConnection = false;
                                }
                            }
                            qnxStream = null;
                        }
                        LogMessage("Closing client connection...");
                        client.Close();
                        LogMessage("Closing client connection... closed");
                        //run = false;
                    }
                    else
                    {
                        Thread.Sleep(1000);
                    }
                }
                listener.Stop();
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

        void ProcessMessageBuffer(ref String data)
        {
            Boolean done = false;
            while ((data.Length > 0) && !done)
            {
                try
                {
                    Int32 messageStart = data.IndexOf((char)0x02);
                    Int32 messageEnd = data.IndexOf((char)0x03);
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
                            String message = data.Substring(messageStart, ((messageEnd - messageStart) + 1));
                            ProcessIndividualMessage(message);
                            // remove processed message from buffer
                            // data.Remove(0, messageEnd + 1);
                            data = data.Substring(messageEnd + 1);
                        }
                        else if (messageEnd < messageStart)
                        {
                            // trash fragment
                            // data.Remove(0, messageEnd + 1);
                            data = data.Substring(messageEnd + 1);
                        }
                    }
                    else
                    {
                        if (messageEnd != -1)
                        {
                            // trash fragment
                            // data.Remove(0, messageEnd + 1);
                            data = data.Substring(messageEnd + 1);
                        }
                        else
                        {
                            // wait for message to come in
                            done = true;
                        }
                    }
                }
                catch
                {
                    // wait for message to complete
                    done = true;
                }
            }
        }

        void ProcessIndividualMessage(String message)
        {
            // Remove Start Byte and End Byte
            String messageString = message.Substring(1, message.Length - 2);
            // Validate checksum
            UInt16 incomingChecksum = Convert.ToUInt16(messageString[messageString.Length - 1]);
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

                            // Get DispositionMessage to update
                            DispositionMessage dispositionMessage = GetDispositionByVin(vehicleVin);
                            if (dispositionMessage.Success.Equals("True", StringComparison.OrdinalIgnoreCase))
                            {
                                Boolean dispositionUpdated = false;

                                foreach (DispositionLine pitch in dispositionMessage.PitchList)
                                {
                                    if (pitch.PitchName == pitchName)
                                    {
                                        LogMessage(String.Format("Updating disposition for existing pitch with PitchName: {0}", pitchName));
                                        pitch.CheckItems = UpdateCheckItems(checkItemString);
                                        dispositionUpdated = true;
                                        break;
                                    }
                                }

                                if (!dispositionUpdated)
                                {
                                    DispositionLine newPitchItem = new DispositionLine();
                                    LogMessage(String.Format("Updating disposition with new pitch with PitchName: {0}", pitchName));
                                    newPitchItem.PitchName = pitchName;
                                    newPitchItem.CheckItems = UpdateCheckItems(checkItemString);
                                    dispositionMessage.PitchList.Add(newPitchItem);
                                }

                                if (!UpdateDisposition(dispositionMessage))
                                {
                                    LogMessage(String.Format("Could not update disposition for VIN {0}", vehicleVin));
                                }
                                else
                                {
                                    LogMessage(String.Format("Updated disposition for VIN {0}", vehicleVin));
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
                            String vehicleVin = fields[1];
                            String optionString = String.Empty;
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
                        else
                        {
                            LogMessage("Invalid build request");
                            SendNak();
                        }
                    }
                    else
                    {
                        LogMessage("Invalid request");
                    }
                }
            }
            else
            {
                LogMessage(String.Format("Not processing message: Invalid checksum (calculated: {0}, received: {1})", ourChecksum, incomingChecksum));
            }
        }

        private void SendNak()
        {
            String nakMessage = String.Format("{0}", Convert.ToChar(0x15));
            SendVehicleBuildResponse(nakMessage);
        }

        private void SendVehicleBuildResponse(string optionString)
        {
            if (qnxStream == null)
            {
                LogMessage("Cannot send vehicle build response: null stream reference");
            }
            else
            {
                using (NetworkStream stream = qnxStream)
                {
                    stream.Write(ASCIIEncoding.ASCII.GetBytes(optionString), 0, optionString.Length);
                }
            }
        }

        private string BuildNoInfoString()
        {
            return String.Format("{0}", Convert.ToChar(0x11));
        }

        private string BuildOptionString(OptionCodesMessage optionCodesMessage)
        {
            String ackMessage = String.Format("{0}", Convert.ToChar(0x15));
            String optionString = String.Empty;
            throw new NotImplementedException();
        }

        List<DispositionTest> UpdateCheckItems(String checkItemString)
        {
            List<String> items = new List<String>(checkItemString.Split(new Char[] { '@' }, StringSplitOptions.RemoveEmptyEntries));
            List<DispositionTest> checkItemList = new List<DispositionTest>();

            LogMessage(String.Format("CheckItems: ({0} items)", items.Count));
            LogMessage(String.Format("\t["));

            foreach (String item in items)
            {
                DispositionTest dispositionTestItem = new DispositionTest();
                List<String> itemFields = new List<String>(item.Split(new Char[] { '!' }, StringSplitOptions.RemoveEmptyEntries));
                foreach (String field in itemFields)
                {
                    String[] fieldToValue = field.Split(new Char[] { ',' });
                    LogMessage(String.Format("\t\t{0} : {1}", fieldToValue[0], fieldToValue[1]));
                    switch (fieldToValue[0])
                    {
                        case "\"Code\"":
                            dispositionTestItem.Code = fieldToValue[1];
                            break;
                        case "\"Message\"":
                            dispositionTestItem.Message = fieldToValue[1];
                            break;
                        case "\"Status\"":
                            dispositionTestItem.Status = Decimal.Parse(fieldToValue[1]);
                            break;
                        case "\"Value\"":
                            dispositionTestItem.Value = Decimal.Parse(fieldToValue[1]);
                            break;
                        case "\"Attribute\"":
                            dispositionTestItem.Attribute = fieldToValue[1];
                            break;
                        case "\"UOM\"":
                            dispositionTestItem.UOM = fieldToValue[1];
                            break;
                        case "\"MaxValue\"":
                            dispositionTestItem.MaxValue = Decimal.Parse(fieldToValue[1]);
                            break;
                        case "\"MinValue\"":
                            dispositionTestItem.MinValue = Decimal.Parse(fieldToValue[1]);
                            break;
                        case "\"StartTime\"":
                            dispositionTestItem.StartTime = DateTime.Parse(fieldToValue[1]);
                            break;
                        case "\"EndTime\"":
                            dispositionTestItem.EndTime = DateTime.Parse(fieldToValue[1]);
                            break;
                        case "\"TargetValue\"":
                            dispositionTestItem.TargetValue = Decimal.Parse(fieldToValue[1]);
                            break;
                        case "\"AttributeTarget\"":
                            dispositionTestItem.AttributeTarget = fieldToValue[1];
                            break;
                        default:
                            break;
                    }
                }
                LogMessage("\t]\n");
                checkItemList.Add(dispositionTestItem);
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
                    url, vin, err.Message));
                disposition = new DispositionMessage();
            }

            return disposition;
        }

        OptionCodesMessage GetOptionCodesByVin(String vin)
        {
            String url = String.Format("http://{0}/Quality/Provider/GetOptionCodesByVin?vin={1}", mesHostname, vin);
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "GET";

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
                    url, vin, err.Message));
                optionCodes = new OptionCodesMessage();
            }

            return optionCodes;
        }

        bool UpdateDisposition(DispositionMessage disposition)
        {
            String url = String.Format("http://{0}/Quality/Provider/UpdateDisposition", mesHostname);
            HttpWebRequest webRequest = WebRequest.Create(url) as HttpWebRequest;
            webRequest.Method = "POST";

            Boolean success = true;

            try
            {
                using (StreamWriter writer = new StreamWriter(webRequest.GetRequestStream()))
                {
                    DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(DispositionMessage));
                    MemoryStream requestMemoryStream = new MemoryStream();
                    serializer.WriteObject(requestMemoryStream, disposition);

                    requestMemoryStream.Position = 0;
                    writer.Write(requestMemoryStream.ToArray());
                }

                using (HttpWebResponse webResponse = webRequest.GetResponse() as HttpWebResponse)
                {
                    if (webResponse.StatusCode != HttpStatusCode.OK)
                    {
                        LogMessage(String.Format("MES Data Interface: Server error (HTTP {0}: {1}).",
                            webResponse.StatusCode, webResponse.StatusDescription));
                        success = false;
                    }

                    DataContractJsonSerializer serializer = new DataContractJsonSerializer(typeof(DispositionMessage));
                    MemoryStream responseMemoryStream = new MemoryStream();
                    webResponse.GetResponseStream().CopyTo(responseMemoryStream);

                    responseMemoryStream.Position = 0;
                    StreamReader reader = new StreamReader(responseMemoryStream);
                    LogMessage(String.Format("MES Data Interface: Response received from host for last sent result - {0}", reader.ReadToEnd()));

                    responseMemoryStream.Position = 0;
                    Object responseObj = serializer.ReadObject(responseMemoryStream);
                    DispositionMessage dispositionResponse = responseObj as DispositionMessage;

                    if (dispositionResponse.Success == null || dispositionResponse.Success.Length == 0)
                    {
                        LogMessage(String.Format("Invalid response from host for last sent result for VIN {0} - Success field is null or empty!", disposition.Vin));
                        success = false;
                    }
                    else if (dispositionResponse.Success.Equals("True", StringComparison.OrdinalIgnoreCase))
                    {
                        LogMessage(String.Format("Successfully send result to host for VIN {0}", disposition.Vin));
                        success = true;
                    }
                    else
                    {
                        LogMessage(String.Format("Error received from host for last sent result for VIN {0} - {1}", disposition.Vin, dispositionResponse.Error));
                        success = false;
                    }
                }
            }
            catch (Exception err)
            {
                LogMessage(String.Format("MES Data Interface: Exception in UpdateDisposition for URL ({0}): {1}",
                    url, err.Message));
                success = false;
            }

            return success;
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
