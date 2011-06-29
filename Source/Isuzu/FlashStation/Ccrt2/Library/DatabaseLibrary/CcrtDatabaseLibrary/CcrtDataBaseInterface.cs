using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Linq;
using System.Data.Linq.Mapping;
using System.Data.SqlClient;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CcrtDb;
using InputDataItemLibrary;
using SerialPortLibrary;


namespace CcrtDataBaseLibrary
{
    public class CcrtDataBaseInterface
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Create a new database interface.  
        /// The object will connect to the specified database when created.
        /// </summary>
        /// <param name="databaseName">Name of the database to connect to.</param>
        public CcrtDataBaseInterface(String databaseName)
        {   // Store the name of the database to connect to
            DatabaseName = databaseName;
            ConnectToDatabase();
            // Create a new viewer form
            m_dbViewer = new DatabaseViewerForm();
        }

        /// <summary>
        /// Connect to the specified database.
        /// </summary>
        /// <param name="name"></param>
        public void ConnectToDatabase()
        {   // Create a new database connection
            DbConnection = new CcrtDatabase(@DatabaseName);
        }

        /// <summary>
        /// Get the list of devices that belong to the specified server.
        /// </summary>
        /// <param name="serverName">Server to retrieve serial devices for.</param>
        /// <returns>List of device names for the specified server.</returns>
        public List<String> GetSerialDeviceNames(String serverName)
        {
            List<String> deviceNames = new List<String>();
            // Get all the device names listed in the port settings table
            Table<CcrtDb.PortSettings> portSettings = DbConnection.GetTable<CcrtDb.PortSettings>();
            var query =
                from setting in portSettings
                where setting.ServerName.Equals(serverName)
                select setting;
            // Save the device names to the list
            foreach (var setting in query)
            {
                deviceNames.Add(setting.DeviceName);
            }
            // Return the list of devices to the caller
            return deviceNames;
        }
                
        /// <summary>
        /// Get all the port settings for a given server name.
        /// </summary>
        /// <param name="serverName">Name of the server to retrieve port settings for.</param>
        /// <returns>Port settings for the specified server.</returns>
        public List<SerialPortLibrary.PortSettings> ReadPortSettings(String serverName)
        {
            Table<CcrtDb.PortSettings> portSettings = DbConnection.GetTable<CcrtDb.PortSettings>();
            var query =
                from setting in portSettings
                where setting.ServerName.Equals(serverName)
                select setting;
            // Place the results into a list of port settings
            List<SerialPortLibrary.PortSettings> serialPortSettings = new List<SerialPortLibrary.PortSettings>();
            foreach (var setting in query)
            {   // Add the new item to the list
                serialPortSettings.Add(BuildPortSettingsFromDb(setting));
            }
            // Return the settings read from the database
            return serialPortSettings;
        }

        /// <summary>
        /// Get the port settings for a specified device.
        /// </summary>
        /// <param name="serverName">Name of the server requesting the data.</param>
        /// <param name="deviceName">Device name to retrieve the serial port settings for.</param>
        /// <returns>Serial port settings for the specified device.</returns>
        public SerialPortLibrary.PortSettings ReadPortSettings(String serverName, String deviceName)
        {
            CcrtDb.PortSettings portSettings = (
                from setting in DbConnection.PortSettings
                where setting.ServerName.Equals(serverName) && setting.DeviceName.Equals(deviceName)
                select setting
            ).First();
            // Only one record should have been returned, build the port settings
            return BuildPortSettingsFromDb(portSettings);
        }

        /// <summary>
        /// Add a new serial port setting to the database for the specified device name.
        /// </summary>
        /// <param name="serverName">Name of the server storing the port settings.</param>
        /// <param name="deviceName">Device name that owns the port settings.</param>
        /// <param name="portSettings">Serial port settings for the device.</param>
        public void AddSerialPortSettings(String serverName, String deviceName, 
                                          SerialPortLibrary.PortSettings portSettings)
        {
            CcrtDb.PortSettings dbSettings = BuildDbPortSettings(serverName, deviceName, portSettings);
            DbConnection.PortSettings.InsertOnSubmit(dbSettings);
            try
            {
                DbConnection.SubmitChanges();
            }
            catch (DuplicateKeyException excpt)
            {
                MessageBox.Show("Device name " + deviceName + " already exists, choose a different name" +
                                Environment.NewLine + excpt.Message, "Device Name Conflict",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                DbConnection.PortSettings.DeleteOnSubmit(dbSettings);
                DbConnection.SubmitChanges();
            }
        }

        /// <summary>
        /// Update the port settings for the specified device.
        /// </summary>
        /// <param name="serverName">Name of the server the device belongs to.</param>
        /// <param name="deviceName"></param>
        /// <param name="newPortSettings"></param>
        public void UpdateSerialPortSettings(String serverName, String deviceName,
                                             SerialPortLibrary.PortSettings newPortSettings)
        {   // Get the entry for the specified device
            var updateItem =
                (from item in DbConnection.PortSettings
                 where item.ServerName.Equals(serverName) && item.DeviceName.Equals(deviceName)
                 select item).First();
            // Save all the settings
            updateItem.BaudRate = newPortSettings.BaudRate;
            updateItem.DataBits = newPortSettings.DataBits;
            updateItem.MonitorData = newPortSettings.MonitorPortForData;
            updateItem.Parity = CcrtSerialPort.ConvertFromParity(newPortSettings.DataParity);
            updateItem.PortName = newPortSettings.PortName;
            updateItem.RxTimeout = newPortSettings.ReadTimeout;
            updateItem.StopBits = CcrtSerialPort.ConvertFromStopBit(newPortSettings.DataStopBits);
            updateItem.TxTimeout = newPortSettings.WriteTimeout;
            // Write the updates back to the database
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Remove the port settings for the specified device.
        /// </summary>
        /// <param name="serverName">Name of the server the device belongs to.</param>
        /// <param name="deviceName">Device to remove serial port settings for.</param>
        public void RemoveSerialPortSetting(String serverName, String deviceName)
        {   // Get the port settings from the database
            var removeItem =
                (from item in DbConnection.PortSettings
                 where item.ServerName.Equals(serverName) && item.DeviceName.Equals(deviceName)
                 select item).First();
            // Delete the entry from the database
            DbConnection.PortSettings.DeleteOnSubmit(removeItem);
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Read all data items that have been configured for the specified input device.
        /// </summary>
        /// <param name="deviceName">Retrieve the data items configured for this Input Device.</param>
        /// <returns>List of data items configured for the specified device.</returns>
        public List<InputDataItem> ReadDataItemSettings(String deviceName)
        {   // Create a query to retrieve the data items for the specified device
            Table<CcrtDb.InputDataItems> dataSettingsTable = DbConnection.GetTable<CcrtDb.InputDataItems>();
            var query =
                from setting in dataSettingsTable
                where setting.DeviceName.Equals(deviceName)
                select setting;
            // Place the results into the list to be returned to the calling function
            List<InputDataItem> items = new List<InputDataItem>();
            foreach (var setting in query)
            {   // Add the data item to the list
                items.Add(BuildInputDataItemFromDb(setting));
            }
            // Return the settings retrieved from the database
            return items;
        }

        /// <summary>
        /// Read the specified data item settings for the specified device.
        /// </summary>
        /// <param name="deviceName">Name of the device to retrieve data items for.</param>
        /// <param name="dataItem">Specific data item to retrieve.</param>
        /// <returns>InputDataItem object for the specified data item belonging to the specified device.</returns>
        public InputDataItem ReadDataItemSettings(String deviceName, String dataItem)
        {
            CcrtDb.InputDataItems item = (
                from setting in DbConnection.InputDataItems
                where setting.DeviceName.Equals(deviceName) && setting.DataName.Equals(dataItem)
                select setting).First();
            return BuildInputDataItemFromDb(item);
        }

        /// <summary>
        /// Add a new data item setting for the specified input device.
        /// </summary>
        /// <param name="deviceName">Name of the device to add the data item setting to.</param>
        /// <param name="dataItem">Data item settings to add to the input device.</param>
        public void AddDataItemSettings(String deviceName, InputDataItem dataItem)
        {
            CcrtDb.InputDataItems dbSettings = BuildDbDataItem(deviceName, dataItem);
            DbConnection.InputDataItems.InsertOnSubmit(dbSettings);
            try
            {
                DbConnection.SubmitChanges();
            }
            catch (DuplicateKeyException excpt)
            {
                MessageBox.Show("Data item " + dataItem.ItemSettings.DataName + " already exists, choose a different name" +
                                Environment.NewLine + excpt.Message, "Data Item Name Conflict",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                DbConnection.InputDataItems.DeleteOnSubmit(dbSettings);
                DbConnection.SubmitChanges();
            }
        }

        /// <summary>
        /// Update the data item settings for the specified input device.
        /// </summary>
        /// <param name="deviceName">Name of the input device to update settings for.</param>
        /// <param name="dataItem">Updated data item settings.</param>
        public void UpdateDataItemSettings(String deviceName, InputDataItem dataItem)
        {   // Get the entry for the specified data item
            var updateItem =
                (from item in DbConnection.InputDataItems
                 where item.DeviceName.Equals(deviceName) && item.DataName.Equals(dataItem.ItemSettings.DataName)
                 select item).First();
            // Save all the settings
            updateItem = BuildDbDataItem(deviceName, dataItem);
            // Write the updates back to the database
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Remove the specified data item from the database for the given input device.
        /// </summary>
        /// <param name="deviceName">Name of the input device to remove dat aitems from.</param>
        /// <param name="dataItemName">Name of the data item to remove.</param>
        public void RemoveDataItemSetting(String deviceName, String dataItemName)
        {   // Get the setting from the database
            var removeItem =
                (from item in DbConnection.InputDataItems
                 where item.DeviceName.Equals(deviceName) && item.DeviceName.Equals(dataItemName)
                 select item).First();
            // Delete the entry from the database
            DbConnection.InputDataItems.DeleteOnSubmit(removeItem);
            DbConnection.SubmitChanges();
        }




        /// <summary>
        /// Get the list of j2534 comm channels that belong to the specified device.
        /// </summary>
        /// <param name="deviceName">device to retrieve comm channel names for.</param>
        /// <returns>List of communication channel names for the specified device.</returns>
        public List<String> GetCommChannelNames(String deviceName)
        {
            List<String> commChannelNames = new List<String>();
            // Get all the comm channels associated with the specified device
            Table<CcrtDb.J2534DeviceSettings> j2534Settings = DbConnection.GetTable<CcrtDb.J2534DeviceSettings>();
            var query =
            from setting in j2534Settings
            where setting.DeviceName.Equals(deviceName)
            select setting;
            // Save the comm channel names to the list
            foreach (var setting in query)
            {
                commChannelNames.Add(setting.CommChannelName);
            }
            // Return the list of comm channel names to the caller
            return commChannelNames;
        }
        /// <summary>
        /// Get the list of j2534 comm channels that belong to the specified device.
        /// </summary>
        /// <param name="deviceName">device to retrieve comm channel names for.</param>
        /// <returns>List of communication channel names for the specified device.</returns>
        public List<String> GetCommChannelNames()
        {
            List<String> commChannelNames = new List<String>();
            // Get all the comm channels associated with the specified device
            Table<CcrtDb.J2534DeviceSettings> j2534Settings = DbConnection.GetTable<CcrtDb.J2534DeviceSettings>();
            // Save the comm channel names to the list
            foreach (CcrtDb.J2534DeviceSettings setting in j2534Settings)
            {
                commChannelNames.Add(setting.CommChannelName);
            }
            // Return the list of comm channel names to the caller
            return commChannelNames;
        }

        /// <summary>
        /// Get all the comm channel settings for a given device.
        /// </summary>
        /// <param name="deviceName">Name of the device to retrieve comm channel settings for.</param>
        /// <returns>Comm channel settings for the specified server.</returns>
        public List<J2534ChannelLibrary.CcrtJ2534Channel> ReadCommChannelSettings(String deviceName)
        {
            Table<CcrtDb.J2534DeviceSettings> j2534Settings = DbConnection.GetTable<CcrtDb.J2534DeviceSettings>();
            var query =
                from setting in j2534Settings
                where setting.DeviceName.Equals(deviceName)
                select setting;
            // Place the results into a list of port settings
            List<J2534ChannelLibrary.CcrtJ2534Channel> commChannel = new List<J2534ChannelLibrary.CcrtJ2534Channel>();
            foreach (var setting in query)
            {   // Add the new item to the list
                commChannel.Add(BuildCommChannelSettingsFromDb(setting));
            }
            // Return the comm channels read from the database
            return commChannel;
        }
        /// <summary>
        /// Get all the comm channel settings
        /// </summary>
        /// <returns>Comm channel settings for the specified server.</returns>
        public List<J2534ChannelLibrary.CcrtJ2534Channel> ReadCommChannelSettings()
        {
            Table<CcrtDb.J2534DeviceSettings> j2534Settings = DbConnection.GetTable<CcrtDb.J2534DeviceSettings>();
            // Place the results into a list of port settings
            List<J2534ChannelLibrary.CcrtJ2534Channel> commChannels = new List<J2534ChannelLibrary.CcrtJ2534Channel>();
            foreach (J2534DeviceSettings setting in j2534Settings)
            {   // Add the new item to the list
                commChannels.Add(BuildCommChannelSettingsFromDb(setting));
            }
            // Return the comm channels read from the database
            return commChannels;
        }

        /// <summary>
        /// Get the comm channel settings for a specified device.
        /// </summary>
        /// <param name="deviceName">Name of the device requesting the data.</param>
        /// <param name="commChannelName">Comm channel name to retrieve settings for.</param>
        /// <returns>J2534 settings for the specified device.</returns>
        public J2534ChannelLibrary.CcrtJ2534Channel ReadCommChannelSettings(String deviceName, String commChannelName)
        {
            CcrtDb.J2534DeviceSettings j2534Settings = (
                from setting in DbConnection.J2534DeviceSettings
                where setting.DeviceName.Equals(deviceName) && setting.CommChannelName.Equals(commChannelName)
                select setting
            ).First();
            // Only one record should have been returned, build the comm channel settings
            return BuildCommChannelSettingsFromDb(j2534Settings);
        }

        /// <summary>
        /// Add a new comm channel setting to the database for the specified device name.
        /// </summary>
        /// <param name="deviceName">Name of the j2534 device.</param>
        /// <param name="commChannelSettings">Settings for the comm channel.</param>
        public void AddCommChannelSettings(String deviceName, 
                                                J2534ChannelLibrary.CcrtJ2534Channel channelSettings)
        {
            CcrtDb.J2534DeviceSettings dbSettings = BuildDbCommChannelSettings(deviceName, channelSettings);
            DbConnection.J2534DeviceSettings.InsertOnSubmit(dbSettings);
            try
            {
                DbConnection.SubmitChanges();
            }
            catch (DuplicateKeyException excpt)
            {
                MessageBox.Show("Channel name " + channelSettings.ChannelName + " already exists, choose a different name" +
                                Environment.NewLine + excpt.Message, "Channel Name Conflict",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                DbConnection.J2534DeviceSettings.DeleteOnSubmit(dbSettings);
                DbConnection.SubmitChanges();
            }
        }

        /// <summary>
        /// Update the comm channel settings for the specified device comm channel name.
        /// </summary>
        /// <param name="deviceName">Name of device channel belongs to</param>
        /// <param name="newChannelSettings"></param>
        public void UpdateCommChannelSettings(String deviceName,
                                             J2534ChannelLibrary.CcrtJ2534Channel newChannelSettings)
        {   // Get the entry for the specified device
            var updateItem =
                (from item in DbConnection.J2534DeviceSettings
                 where item.DeviceName.Equals(deviceName) && item.CommChannelName.Equals(newChannelSettings.ChannelName)
                 select item).First();
            // Save all the settings
            updateItem.BaudRate = newChannelSettings.GetBaudRate();
            updateItem.ChannelIndex = newChannelSettings.ChannelIndex;
            updateItem.ConnectFlag = newChannelSettings.GetConnectFlag();
            updateItem.ProtocolID = newChannelSettings.GetProtocolID();
            // Write the updates back to the database
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Remove the port settings for the specified device.
        /// </summary>
        /// <param name="deviceName">Name of the device the comm channel belongs to.</param>
        /// <param name="commChannelName">Comm channel to remove settings for.</param>
        public void RemoveCommChannelSetting(String deviceName, String commChannelName)
        {   // Get the port settings from the database
            var removeItem =
                (from item in DbConnection.J2534DeviceSettings
                 where item.DeviceName.Equals(deviceName) && item.CommChannelName.Equals(commChannelName)
                 select item).First();
            // Delete the entry from the database
            DbConnection.J2534DeviceSettings.DeleteOnSubmit(removeItem);
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Display the contents of the data base.
        /// </summary>
        public void DisplayDatabaseViewer()
        {   // Display the port settings
            m_dbViewer.ClearForm();
            Table<CcrtDb.PortSettings> portSettings = DbConnection.GetTable<CcrtDb.PortSettings>();
            var query = from setting in portSettings select setting;
            List<CcrtDb.PortSettings> dbPortSettings = new List<CcrtDb.PortSettings>();
            foreach (var setting in query)
            {
                dbPortSettings.Add(setting);
            }
            m_dbViewer.DisplayDatabaseContents(dbPortSettings);
            // Display the data items
            Table<InputDataItems> dbDataItems = DbConnection.GetTable<InputDataItems>();
            var diQuery = from diSetting in dbDataItems select diSetting;
            List<InputDataItems> dataItems = new List<InputDataItems>();
            foreach(var diSetting in diQuery)
            {
                dataItems.Add(diSetting);
            }
            m_dbViewer.DisplayDatabaseContents(dataItems);
            // Make the form visible
            if (!m_dbViewer.Visible)
            {
                m_dbViewer.Show();
            }
            else
            {   // Form already displayed, bring to the front
                m_dbViewer.BringToFront();
            }
        }

        /// <summary>
        /// Build a SerialPortLibrary type PortSettings type from a database port setting entry.
        /// </summary>
        /// <param name="dbSetting">Port setting data from the database.</param>
        /// <returns>SerialPortLibrary.PortSettings type built from the database data.</returns>
        private SerialPortLibrary.PortSettings BuildPortSettingsFromDb(CcrtDb.PortSettings dbSetting)
        {
            SerialPortLibrary.PortSettings pSetting = new SerialPortLibrary.PortSettings();
            pSetting.BaudRate = Convert.ToUInt16(dbSetting.BaudRate);
            pSetting.DataBits = Convert.ToUInt16(dbSetting.DataBits);
            pSetting.MonitorPortForData = dbSetting.MonitorData;
            pSetting.PortName = dbSetting.PortName;
            pSetting.ReadTimeout = dbSetting.RxTimeout;
            pSetting.WriteTimeout = dbSetting.TxTimeout;
            pSetting.DataStopBits = CcrtSerialPort.ConvertToStopBit(dbSetting.StopBits);
            pSetting.DataParity = CcrtSerialPort.ConvertToParity(dbSetting.Parity);
            return pSetting;
        }

        /// <summary>
        /// Use the provided data to make a database entry for a new port setting.
        /// </summary>
        /// <param name="serverName">Name of the server this port setting belongs to.</param>
        /// <param name="deviceName">Name of the device using the specified serial port settings.</param>
        /// <param name="portSettings">Serial port settings to store in the database.</param>
        /// <returns>Database entry for the port settings.</returns>
        private CcrtDb.PortSettings BuildDbPortSettings(String serverName, String deviceName, 
                                                        SerialPortLibrary.PortSettings portSettings)
        {
            CcrtDb.PortSettings settings = new CcrtDb.PortSettings();
            settings.BaudRate = portSettings.BaudRate;
            settings.DataBits = portSettings.DataBits;
            settings.DeviceName = deviceName;
            settings.MonitorData = portSettings.MonitorPortForData;
            settings.Parity = CcrtSerialPort.ConvertFromParity(portSettings.DataParity);
            settings.PortName = portSettings.PortName;
            settings.RxTimeout = portSettings.ReadTimeout;
            settings.TxTimeout = portSettings.WriteTimeout;
            settings.ServerName = serverName;
            settings.StopBits = CcrtSerialPort.ConvertFromStopBit(portSettings.DataStopBits);
            return settings;
        }

        /// <summary>
        /// Create a new InputDataItem from the database entry.
        /// </summary>
        /// <param name="dbSetting">Data from the database to convert into an InputDataItem.</param>
        /// <returns>InputDataItem object created from the supplied database entry.</returns>
        private InputDataItem BuildInputDataItemFromDb(CcrtDb.InputDataItems dbSetting)
        {
            DataItemSettings settings = new DataItemSettings();
            settings.DataName = dbSetting.DataName;
            settings.MinDataLength = dbSetting.MinLength;
            settings.MaxDataLength = dbSetting.MaxLength;
            settings.HeaderData = dbSetting.HeaderData;
            settings.HeaderDataLength = dbSetting.HeaderLength;
            settings.TrailerData = dbSetting.TrailerData;
            settings.TrailerDataLength = dbSetting.TrailerLength;
            return new InputDataItem(settings);
        }

        /// <summary>
        /// Create a new database 
        /// </summary>
        /// <param name="deviceName"></param>
        /// <param name="item"></param>
        /// <returns></returns>
        private CcrtDb.InputDataItems BuildDbDataItem(String deviceName, InputDataItem item)
        {
            CcrtDb.InputDataItems dbSetting = new CcrtDb.InputDataItems();
            dbSetting.DeviceName = deviceName;
            dbSetting.DataName = item.ItemSettings.DataName;
            dbSetting.MinLength = item.ItemSettings.MinDataLength;
            dbSetting.MaxLength = item.ItemSettings.MaxDataLength;
            dbSetting.HeaderData = item.ItemSettings.HeaderData;
            dbSetting.HeaderLength = item.ItemSettings.HeaderDataLength;
            dbSetting.TrailerData = item.ItemSettings.TrailerData;
            dbSetting.TrailerLength = item.ItemSettings.TrailerDataLength;
            return dbSetting;
        }



        /// <summary>
        /// Build a J2534ChannelLibrary type CommChannel type from a database comm channel setting entry.
        /// </summary>
        /// <param name="dbSetting">Port setting data from the database.</param>
        /// <returns>SerialPortLibrary.PortSettings type built from the database data.</returns>
        private J2534ChannelLibrary.CcrtJ2534Channel BuildCommChannelSettingsFromDb(CcrtDb.J2534DeviceSettings dbSetting)
        {
            J2534ChannelLibrary.CcrtJ2534Channel cSetting = new J2534ChannelLibrary.CcrtJ2534Channel();
            cSetting.SetBaudRate(dbSetting.BaudRate);
            cSetting.ChannelIndex = Convert.ToUInt16(dbSetting.ChannelIndex);
            cSetting.ChannelName = dbSetting.CommChannelName;
            cSetting.SetConnectFlag(dbSetting.ConnectFlag);
            cSetting.SetProtocolID(dbSetting.ProtocolID);

            return cSetting;
        }

        /// <summary>
        /// Use the provided data to make a database entry for a new comm channel.
        /// </summary>
        /// <param name="deviceName">Name of the device to add a comm channel for.</param>
        /// <param name="commChannelSettings">Comm Channel settings to store in the database.</param>
        /// <returns>Database entry for the comm channel settings.</returns>
        private CcrtDb.J2534DeviceSettings BuildDbCommChannelSettings(String deviceName, J2534ChannelLibrary.CcrtJ2534Channel commChannelSettings)
        {
            CcrtDb.J2534DeviceSettings settings = new CcrtDb.J2534DeviceSettings();
            settings.BaudRate = commChannelSettings.GetBaudRate();
            settings.ChannelIndex = commChannelSettings.ChannelIndex;
            settings.CommChannelName = commChannelSettings.ChannelName;
            settings.ConnectFlag = commChannelSettings.GetConnectFlag();
            settings.DeviceName = deviceName;
            settings.ProtocolID = commChannelSettings.GetProtocolID();
            return settings;
        }


        //-----------------------------------------------------------------------------------------
        // Class Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Get/Set the name of the data base to connect to.
        /// </summary>
        private String m_databaseName;
        private String DatabaseName
        {
            get { return m_databaseName; }
            set { m_databaseName = value; }
        }

        /// <summary>
        /// Connection to the local database.
        /// </summary>
        private static CcrtDatabase m_databaseConnection;
        private static CcrtDatabase DbConnection
        {
            get { return m_databaseConnection; }
            set { m_databaseConnection = value; }
        }

        /// <summary>
        /// Database viewer form so we can see the contents of the database.
        /// </summary>
        private DatabaseViewerForm m_dbViewer;
    }
}
