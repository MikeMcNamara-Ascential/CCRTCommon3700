using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using J2534ChannelLibrary;
using J2534DotNet;
using Logger;

namespace ModuleCommServer
{
    [Serializable, XmlRoot("AbsModules")]
    public class BrakeModule
    {
        /// <summary>
        /// Name of the ABS module.
        /// </summary>
        [XmlAttribute(AttributeName="ModuleName")]
        public String ModuleName { get; set; }

        /// <summary>
        /// ABS module communication protocol.
        /// </summary>
        [XmlAttribute(AttributeName="Protocol")]
        public String CommProtocol { get; set; }

        /// <summary>
        /// Communication bus for the ABS module communications.
        /// </summary>
        [XmlAttribute(AttributeName="Bus")]
        public String CommBus { get; set; }

        /// <summary>
        /// Configuration file for the ABS module.
        /// </summary>
        [XmlAttribute(AttributeName="ConfigFile")]
        public String ConfigFile { get; set; }

        /// <summary>
        /// The keep alive interval for the module
        /// </summary>
        [XmlAttribute(AttributeName = "KeepAliveInterval")]
        public int KeepAliveInterval { get; set; }

        /// <summary>
        /// Message table for the current module.
        /// </summary>
        [XmlIgnoreAttribute]
        public Dictionary<String, CcrtJ2534Defs.ECUMessage> MessageTable { get; set; }

        private CcrtLogger Log { get; set; }
        public void setLogger(CcrtLogger logger)
        {
            Log = logger;
            Log.Log("Logger set in BrakeModule.cs");
        }
        

        /// <summary>
        /// Load the message table.
        /// </summary>
        public void LoadMessageTable()
        {   // Load the XML config file that contains the message table
            Log.Log("LoadMessageTable() - Enter");
            XmlDocument configFile = new XmlDocument();
            Log.Log("Loading config file");
            try
            {
                configFile.Load(ConfigFile);
            }
            catch (Exception e)
            {
                Log.Log("Error loading config file: " + e.ToString());
            }
            Log.Log("Loaded config file: " + configFile.BaseURI);
            MessageTable = new Dictionary<String, CcrtJ2534Defs.ECUMessage>();
            // Find the relevant major nodes
            XmlNode xmitMessages = configFile.FirstChild.SelectSingleNode("SerialMessages").SelectSingleNode("TransmitMessages");
            Log.Log("LoadMessageTable 1");
            XmlNodeList vehicleMessages = configFile.FirstChild.SelectSingleNode("SerialMessages").SelectSingleNode("VehicleMessages").ChildNodes;
            KeepAliveInterval = Convert.ToInt32(configFile.FirstChild.SelectSingleNode("Setup/ModuleKeepAliveTime").FirstChild.Value);
            Log.Log("LoadMessageTable 2");
            // Get the module response and request IDs
            List<Byte> requestId = new List<Byte>();
            List<Byte> responseId = new List<Byte>();
            Log.Log("LoadMessageTable 3");
            UInt16 reqId = Convert.ToUInt16(configFile.FirstChild.SelectSingleNode("Setup/ModuleRequestID").FirstChild.Value, 16);
            Log.Log("LoadMessageTable 4");
            //requestId.Add((Byte)((reqId & 0xFF00) >> 8));
            requestId.Add((Byte)(reqId & 0x00FF));
            UInt16 respId = Convert.ToUInt16(configFile.FirstChild.SelectSingleNode("Setup/ModuleResponseID").FirstChild.Value, 16);
            Log.Log("LoadMessageTable 5");
            //responseId.Add((Byte)((respId & 0xFF00) >> 8));
            responseId.Add((Byte)(respId & 0x00FF));
            // Create the message table
            foreach (XmlNode msg in vehicleMessages)
            {
                try
                {
                    if (msg.NodeType == XmlNodeType.Element)
                    {
                        CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
                        message.m_messageFilter.requestID = requestId.ToArray();
                        message.m_messageFilter.responseID = responseId.ToArray();
                        String xmitMsgName = msg.SelectSingleNode("TransmitMessage").FirstChild.Value;
                        String[] rawMsgBytes = xmitMessages.SelectSingleNode(xmitMsgName).FirstChild.Value.Split(',');
                        foreach (String rawByte in rawMsgBytes)
                        {
                            message.m_txMessage.Add(Convert.ToByte(rawByte, 16));
                        }
                        // Add it all to the dictionary
                        MessageTable.Add(msg.Name, message);
                    }
                }
                catch (XmlException e)
                {
                    // message table not loaded correctly
                }
            }
        }
    }
}
