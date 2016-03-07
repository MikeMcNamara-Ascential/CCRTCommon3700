using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CcrtDb;
using DbLinq.Data.Linq;
using DbLinq.Data.Linq.Mapping;
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
            String connectString = ("DbLinqProvider=sqlite;" +
                                    "DbLinqConnectionType=System.Data.SQLite.SQLiteCOnnection, System.Data.SQLite;" +
                                    "Data Source=" + DatabaseName);
            DbConnection = new CcrtDb.Main(new SQLiteConnection(connectString));
        }

        /// <summary>
        /// Close the connection to the database.
        /// </summary>
        public void CloseDatabaseConnection()
        {
            DbConnection.Connection.Close();
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
                where setting.ServerName == serverName
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
        /// Get all configured port settings.
        /// </summary>
        /// <returns>List of all configured ports.</returns>
        public List<SerialPortLibrary.PortSettings> ReadPortSettings()
        {   // Get the port settings from the database
            Table<CcrtDb.PortSettings> portSettings = DbConnection.GetTable<CcrtDb.PortSettings>();
            var query =
                from setting in portSettings
                select setting;
            // place the results into a list of port settings
            List<SerialPortLibrary.PortSettings> serialPortSettings = new List<SerialPortLibrary.PortSettings>();
            foreach (var setting in query)
            {   // Add the item to the list
                serialPortSettings.Add(BuildPortSettingsFromDb(setting));
            }
            // Return the settings read from the database
            return serialPortSettings;
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
                where setting.ServerName == serverName
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
                where (setting.ServerName == serverName) && (setting.DeviceName == deviceName)
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
            catch (SQLiteException excpt)
            {
                MessageBox.Show("Device name " + deviceName + " already exists, choose a different name" +
                                Environment.NewLine + excpt.Message, "Device Name Conflict",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                DbConnection.PortSettings.DeleteOnSubmit(dbSettings);
                DbConnection.SubmitChanges();
            }
            catch (System.Data.Linq.DuplicateKeyException excpt)
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
                 where (item.ServerName == serverName) && (item.DeviceName == deviceName)
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
            updateItem.DeviceType = newPortSettings.DeviceType;
            updateItem.HeaderData = newPortSettings.HeaderData;
            updateItem.HeaderLength = newPortSettings.HeaderDataLength;
            updateItem.TrailerData = newPortSettings.TrailerData;
            updateItem.TrailerLength = newPortSettings.TrailerDataLength;
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
                 where (item.ServerName == serverName) && (item.DeviceName == deviceName)
                 select item).First();
            // Delete the entry from the database
            DbConnection.PortSettings.DeleteOnSubmit(removeItem);
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Read all input data items that have been configured.
        /// </summary>
        /// <returns>List of all configured input data items.</returns>
        public List<InputDataItem> ReadDataItemSettings()
        {   // Create a query to retrieve the data items
            Table<CcrtDb.InputDataItems> dataSettingsTable = DbConnection.GetTable<CcrtDb.InputDataItems>();
            var query =
                from setting in dataSettingsTable
                select setting;
            // Place the results in the list to be returned to the calling function
            List<InputDataItem> items = new List<InputDataItem>();
            foreach (var setting in query)
            {   // Add t he data item to the list
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
        public InputDataItem ReadDataItemSettings(/*String deviceName,*/ String dataItem)
        {
            CcrtDb.InputDataItems item = (
                from setting in DbConnection.InputDataItems
                where /*(setting.DeviceName == deviceName) &&*/ (setting.DataName == dataItem)
                select setting).First();
            return BuildInputDataItemFromDb(item);
        }

        /// <summary>
        /// Add a new data item setting for the specified input device.
        /// </summary>
        /// <param name="dataItem">Data item settings to add to the input device.</param>
        public void AddDataItemSettings(InputDataItem dataItem)
        {
            CcrtDb.InputDataItems dbSettings = BuildDbDataItem(dataItem);
            DbConnection.InputDataItems.InsertOnSubmit(dbSettings);
            try
            {
                DbConnection.SubmitChanges();
            }
            catch (SQLiteException excpt)
            {
                MessageBox.Show("Data item " + dataItem.ItemSettings.DataName + " already exists, choose a different name" +
                                Environment.NewLine + excpt.Message, "Data Item Name Conflict",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                DbConnection.InputDataItems.DeleteOnSubmit(dbSettings);
                DbConnection.SubmitChanges();
            }
            catch (System.Data.Linq.DuplicateKeyException excpt)
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
        public void UpdateDataItemSettings(InputDataItem dataItem)
        {   // Get the entry for the specified data item
            var updateItem =
                (from item in DbConnection.InputDataItems
                 where (item.DataName == dataItem.ItemSettings.DataName)
                 select item).First();
            // Save all the settings
            updateItem = BuildDbDataItem(dataItem);
            // Write the updates back to the database
            DbConnection.SubmitChanges();
        }

        /// <summary>
        /// Remove the specified data item from the database for the given input device.
        /// </summary>
        /// <param name="deviceName">Name of the input device to remove dat aitems from.</param>
        /// <param name="dataItemName">Name of the data item to remove.</param>
        public void RemoveDataItemSetting(/*String deviceName,*/ String dataItemName)
        {   // Get the setting from the database
            var removeItem =
                (from item in DbConnection.InputDataItems
                 where (item.DataName == dataItemName)
                 select item).First();
            // Delete the entry from the database
            DbConnection.InputDataItems.DeleteOnSubmit(removeItem);
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
            pSetting.HeaderData = dbSetting.HeaderData;
            pSetting.HeaderDataLength = dbSetting.HeaderLength;
            pSetting.TrailerData = dbSetting.TrailerData;
            pSetting.TrailerDataLength = Convert.ToInt32(dbSetting.TrailerLength);
            pSetting.DeviceType = dbSetting.DeviceType;
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
            settings.HeaderData = portSettings.HeaderData;
            settings.HeaderLength = portSettings.HeaderDataLength;
            settings.TrailerData = portSettings.TrailerData;
            settings.TrailerLength = portSettings.TrailerDataLength;
            settings.DeviceType = portSettings.DeviceType;
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
            return new InputDataItem(settings);
        }

        /// <summary>
        /// Create a new database entry for an input data item.
        /// </summary>
        /// <param name="item">User configured input data item.</param>
        /// <returns>CCRT database entry for an input data item.</returns>
        private CcrtDb.InputDataItems BuildDbDataItem(InputDataItem item)
        {
            CcrtDb.InputDataItems dbSetting = new CcrtDb.InputDataItems();
            dbSetting.DataName = item.ItemSettings.DataName;
            dbSetting.MinLength = item.ItemSettings.MinDataLength;
            dbSetting.MaxLength = item.ItemSettings.MaxDataLength;
            return dbSetting;
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
        private static CcrtDb.Main m_databaseConnection;
        private static CcrtDb.Main DbConnection
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
