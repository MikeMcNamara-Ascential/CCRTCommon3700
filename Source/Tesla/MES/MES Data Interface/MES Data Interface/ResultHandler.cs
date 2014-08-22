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

namespace MES_Data_Interface
{
    public delegate void LogMessageDelegate(String message);

    public delegate void ListenerClosedEventHandler(Object sender, EventArgs e);

    class ResultHandler
    {
        String qnxIpAddress;
        String qnxListenerIpAddress;
        String qnxListenerTcpPort;
        String mesHostname;
        Boolean requestCloseListener;
        LogMessageDelegate LogMessage;
        public Boolean ListenerActive { get; private set; }
        public event ListenerClosedEventHandler ListenerClosed;

        public ResultHandler(Settings settings, LogMessageDelegate function)
        {
            LogMessage = new LogMessageDelegate(function);
            RestartListener(settings);
        }

        public void RestartListener(Settings newSettings)
        {
            qnxIpAddress = newSettings.QnxIpAddress;
            qnxListenerIpAddress = newSettings.QnxListenerIpAddress;
            qnxListenerTcpPort = newSettings.QnxListenerTcpPort;
            mesHostname = newSettings.MesHostName;

            RestartListener();
        }

        public void RestartListener()
        {
            LogMessage("Starting Listener Thread...");
            SetListenerActive(true);
            Thread listenerThread = new Thread(new ThreadStart(RunListener));

            listenerThread.Start();
        }

        public void CloseListener()
        {
            requestCloseListener = true;
        }

        void SetListenerActive(Boolean active)
        {
            requestCloseListener = false;
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

                while (!requestCloseListener)
                {
                    if (listener.Pending())
                    {
                        LogMessage("Accepting client connection...");
                        TcpClient client = listener.AcceptTcpClient();

                        using (NetworkStream stream = client.GetStream())
                        {
                            while (keepConnection && !requestCloseListener)
                            {
                                try
                                {
                                    // Read available data into buffer
                                    Int32 i = 0;

                                    i = stream.Read(bytes, 0, bytes.Length);

                                    if (i != 0)
                                    {
                                        data += Encoding.ASCII.GetString(bytes, 0, i);
                                    }
                                    else
                                    {
                                        Thread.Sleep(500);
                                        keepConnection = client.Connected;
                                        data = String.Empty;
                                    }
                                    // Process complete messages from buffer
                                    ProcessMessageBuffer(ref data);
                                }
                                catch
                                {
                                    keepConnection = false;
                                }
                            }
                        }
                        LogMessage("Closing client connection...");
                        client.Close();
                        LogMessage("Closing client connection... closed");
                        //run = false;
                    }
                    else
                    {
                        LogMessage("No incoming client connections waiting...");
                        Thread.Sleep(2000);
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
                            data.Remove(0, messageEnd + 1);
                        }
                        else if (messageEnd < messageStart)
                        {
                            // trash fragment
                            data.Remove(0, messageEnd + 1);
                        }
                    }
                    else
                    {
                        if (messageEnd != -1)
                        {
                            // trash fragment
                            data.Remove(0, messageEnd + 1);
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
            String resultMessageString = message.Substring(1, message.Length - 1);
            List<String> fields = new List<String>(resultMessageString.Split(new Char[] { ',' }));
            if (fields.Count == 4)
            {
                String vehicleVin = fields[0];
                String successStatus = fields[1];
                String pitchName = fields[2];
                String checkItemString = fields[3];
                LogMessage("Processing result for VIN " + vehicleVin);

                // Get DispositionMessage to update
                DispositionMessage dispositionMessage = GetDispositionByVin(vehicleVin);
                Boolean dispositionUpdated = false;

                foreach (DispositionLine pitch in dispositionMessage.PitchList)
                {
                    if (pitch.PitchName == pitchName)
                    {
                        pitch.CheckItems = UpdateCheckItems(checkItemString);
                        dispositionUpdated = true;
                        break;
                    }
                }

                if (!dispositionUpdated)
                {
                    DispositionLine newPitchItem = new DispositionLine();
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
                LogMessage("Invalid result message");
            }
        }

        List<DispositionTest> UpdateCheckItems(String checkItemString)
        {
            List<String> item = new List<String>(checkItemString.Split(new Char[] { '@' }));
            throw new NotImplementedException();
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
}
